// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownSkillSkyFallSlash.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NetherCrown/Character/NetherCrownPlayerController.h"
#include "NetherCrown/Character/AnimInstance/NetherCrownKnightAnimInstance.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"

#define DEBUG_SPHERE 0

void UNetherCrownSkillSkyFallSlash::InitSkillObject()
{
	Super::InitSkillObject();

	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	USkeletalMeshComponent* SkeletalMeshComponent{ SkillOwnerCharacter->GetMesh() };
	UNetherCrownKnightAnimInstance* NetherCrownKnightAnimInstance{};
	NetherCrownKnightAnimInstance = SkeletalMeshComponent ? Cast<UNetherCrownKnightAnimInstance>(SkeletalMeshComponent->GetAnimInstance()) : nullptr;
	if (!ensureAlways(IsValid(NetherCrownKnightAnimInstance)))
	{
		return;
	}

	NetherCrownKnightAnimInstance->GetOnHitSkyFallSlashSkill().AddUObject(this, &ThisClass::HandleOnHitSkyFallSlashSkill);

	constexpr int32 ArmMaterialIndex{ 1 };

	ArmMaterialInstanceDynamic = SkeletalMeshComponent ? SkeletalMeshComponent->CreateDynamicMaterialInstance(ArmMaterialIndex) : nullptr;
	if (!ensureAlways(IsValid(ArmMaterialInstanceDynamic)))
	{
		return;
	}
}

void UNetherCrownSkillSkyFallSlash::ExecuteSkillGameplay() const
{
	Super::ExecuteSkillGameplay();

	//@NOTE : Only gameplay logic (server)
}

void UNetherCrownSkillSkyFallSlash::PlaySkillCosmetics()
{
	Super::PlaySkillCosmetics();

	//@NOTE : Only cometics logic (all client)
	StartSkillCameraCurveTimer();
	StartSkillArmMaterialParameterCurveTimer();
}

void UNetherCrownSkillSkyFallSlash::StartSkillCameraCurveTimer()
{
	SkillCameraCurveElapsedTime = 0.f;

	const UWorld* World{ GetWorld() };
	check(World);

	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	if (!SkillOwnerCharacter->IsLocallyControlled())
	{
		return;
	}

	World->GetTimerManager().SetTimer(
		SkillCameraCurveTimerHandle,
		this,
		&ThisClass::ApplySkillCameraCurveFloat,
		0.015f,
		true,
		0.f
	);
}

void UNetherCrownSkillSkyFallSlash::StartSkillArmMaterialParameterCurveTimer()
{
	SkillArmMaterialCurveElapsedTime = 0.f;

	const UWorld* World{ GetWorld() };
	check(World);

	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	World->GetTimerManager().SetTimer(SkillArmMaterialCurveTimerHandle, this, &ThisClass::ApplySkillArmMaterialParameterCurveFloat, 0.015f, true, 0.f);
}

void UNetherCrownSkillSkyFallSlash::ApplySkillArmMaterialParameterCurveFloat()
{
	const UWorld* World{ GetWorld() };
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	const UCurveFloat* SkillArmMaterialCurveFloat{ SkillArmMaterialCurveFloatSoft.LoadSynchronous() };
	if (!ensureAlways(IsValid(SkillArmMaterialCurveFloat)) || !ensureAlways(IsValid(World)) || !ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		World->GetTimerManager().ClearTimer(SkillArmMaterialCurveTimerHandle);
		return;
	}

	SkillArmMaterialCurveElapsedTime += 0.01f;

	float MinTime{};
	float MaxTime{};
	SkillArmMaterialCurveFloat->GetTimeRange(MinTime, MaxTime);

	if (SkillArmMaterialCurveElapsedTime > MaxTime)
	{
		World->GetTimerManager().ClearTimer(SkillArmMaterialCurveTimerHandle);
		return;
	}

	const float ArmMaterialCurveFloatValue{ SkillArmMaterialCurveFloat->GetFloatValue(SkillArmMaterialCurveElapsedTime) };
	ArmMaterialInstanceDynamic->SetScalarParameterValue(ArmMaterialScalarParameterName, ArmMaterialCurveFloatValue);
}

void UNetherCrownSkillSkyFallSlash::HandleOnHitSkyFallSlashSkill()
{
	const TArray<AActor*> DetectedActors{ GetSkillDetectedTargets() };
	if (DetectedActors.IsEmpty())
	{
		return;
	}

	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	if (SkillOwnerCharacter->IsLocallyControlled())
	{
		ANetherCrownPlayerController* SkillOwnerController{ Cast<ANetherCrownPlayerController>(SkillOwnerCharacter->GetController()) };
		APlayerCameraManager* CameraManager{ SkillOwnerController ? SkillOwnerController->PlayerCameraManager : nullptr };
		if (!ensureAlways(IsValid(CameraManager)))
		{
			return;
		}

		CameraManager->StartCameraShake(SkillCameraShakeBaseClass, 1.f);
	}

	if (!SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	for (AActor* DetectedActor : DetectedActors)
	{
		if (ANetherCrownEnemy* DetectedEnemy = Cast<ANetherCrownEnemy>(DetectedActor))
		{
			ApplyCrowdControlToTarget(DetectedEnemy, ENetherCrownCrowdControlType::KNOCK_BACK, KnockBackDuration);

			const UNetherCrownCrowdControlComponent* CrowdControlComponent{ DetectedEnemy->GetCrowdControlComponent() };
			if (IsValid(CrowdControlComponent))
			{
				CrowdControlComponent->KnockBack(SkillKnockBackVector);
			}

			UGameplayStatics::ApplyDamage(DetectedEnemy, CalculatePhysicalSkillDamage(), SkillOwnerCharacter->GetController(), SkillOwnerCharacter, UDamageType::StaticClass());

			//@NOTE : Skill Hit Sound is played by TakeDamage function, So only Spawn VFX
			Multicast_SpawnSkillImpactEffect(DetectedEnemy);
		}
	}
}

const TArray<ANetherCrownEnemy*> UNetherCrownSkillSkyFallSlash::GetSkillDetectedTargets() const
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return{};
	}

	TArray<AActor*> OverlappedActors{};

	const FVector& SkillOwnerCharacterLocation{ SkillOwnerCharacter->GetActorLocation() };
	const FVector& SkillOwnerCharacterForwardVector{ SkillOwnerCharacter->GetActorForwardVector() };

	FVector DetectSphereLocation{};
	const FVector& DetectSphereLocationOffset{ SkillOwnerCharacterForwardVector * SkillDetectingSphereRadius / 2.f };
	DetectSphereLocation = SkillOwnerCharacterLocation + DetectSphereLocationOffset;

	const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes{ UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn) };
	const bool bDetectEnemy{ UKismetSystemLibrary::SphereOverlapActors(this, DetectSphereLocation,
		SkillDetectingSphereRadius, ObjectTypes, ANetherCrownEnemy::StaticClass(), TArray<AActor*>(), OverlappedActors) };

#if DEBUG_SPHERE
	UKismetSystemLibrary::DrawDebugSphere(this, DetectSphereLocation, SkillDetectingSphereRadius, 16, FColor::Red, 10.f);
#endif

	if (!bDetectEnemy)
	{
		return {};
	}

	const float CosThreshold = FMath::Cos(FMath::DegreesToRadians(SkillDetectingThresholdDegrees));

	TArray<ANetherCrownEnemy*> DetectedEnemies{};
	for (AActor* OverlappedActor : OverlappedActors)
	{
		const FVector& OverlappedActorLocation{ OverlappedActor->GetActorLocation() };
		FVector DirectionToTarget{ (OverlappedActorLocation - SkillOwnerCharacterLocation) };
		DirectionToTarget.Normalize();

		const double DotResult{ FVector::DotProduct(DirectionToTarget, SkillOwnerCharacterForwardVector) };
		if (DotResult >= CosThreshold)
		{
			DetectedEnemies.Add(Cast<ANetherCrownEnemy>(OverlappedActor));
		}
	}

	return DetectedEnemies;
}

void UNetherCrownSkillSkyFallSlash::ApplySkillCameraCurveFloat()
{
	const UWorld* World{ GetWorld() };
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	const UCurveFloat* SkillCameraCurveFloat{ SkillCameraCurveFloatSoft.LoadSynchronous() };
	if (!ensureAlways(IsValid(SkillCameraCurveFloat)) || !ensureAlways(IsValid(World)) || !ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		World->GetTimerManager().ClearTimer(SkillCameraCurveTimerHandle);
		return;
	}

	if (!SkillOwnerCharacter->IsLocallyControlled())
	{
		return;
	}

	SkillCameraCurveElapsedTime += 1.f;

	float MinTime{};
	float MaxTime{};
	SkillCameraCurveFloat->GetTimeRange(MinTime, MaxTime);

	if (SkillCameraCurveElapsedTime > MaxTime)
	{
		World->GetTimerManager().ClearTimer(SkillCameraCurveTimerHandle);
		return;
	}

	SkillOwnerCharacter->SetSpringArmZOffset(SkillCameraCurveFloat->GetFloatValue(SkillCameraCurveElapsedTime));
}
