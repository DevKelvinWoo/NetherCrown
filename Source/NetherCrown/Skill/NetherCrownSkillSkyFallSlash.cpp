// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownSkillSkyFallSlash.h"

#include "Kismet/KismetSystemLibrary.h"
#include "NetherCrown/Character/NetherCrownPlayerController.h"
#include "NetherCrown/Character/AnimInstance/NetherCrownCharacterAnimInstance.h"
#include "NetherCrown/Character/AnimInstance/NetherCrownKnightAnimInstance.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"

void UNetherCrownSkillSkyFallSlash::InitSkillObject()
{
	Super::InitSkillObject();

	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	const USkeletalMeshComponent* SkeletalMeshComponent{ SkillOwnerCharacter->GetMesh() };
	UNetherCrownKnightAnimInstance* NetherCrownKnightAnimInstance{};
	NetherCrownKnightAnimInstance = SkeletalMeshComponent ? Cast<UNetherCrownKnightAnimInstance>(SkeletalMeshComponent->GetAnimInstance()) : nullptr;
	if (!ensureAlways(IsValid(NetherCrownKnightAnimInstance)))
	{
		return;
	}

	NetherCrownKnightAnimInstance->GetOnHitSkyFallSlashSkill().AddUObject(this, &ThisClass::HandleOnHitSkyFallSlashSkill);
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
}

void UNetherCrownSkillSkyFallSlash::StartSkillCameraCurveTimer()
{
	ElapsedTime = 0.f;

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

void UNetherCrownSkillSkyFallSlash::HandleOnHitSkyFallSlashSkill()
{
	const TArray<AActor*> DetectedActors{ GetSkillDetectedTargets() };
	if (DetectedActors.IsEmpty())
	{
		return;
	}

	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	if (!SkillOwnerCharacter->IsLocallyControlled())
	{
		return;
	}

	for (AActor* DetectedActor : DetectedActors)
	{
		if (ANetherCrownEnemy* DetectedEnemy = Cast<ANetherCrownEnemy>(DetectedActor))
		{
			ApplyKnockBackToTarget(DetectedEnemy, SkillKnockBackVector);
		}
	}

	ANetherCrownPlayerController* SkillOwnerController{ Cast<ANetherCrownPlayerController>(SkillOwnerCharacter->GetController()) };
	APlayerCameraManager* CameraManager{ SkillOwnerController ? SkillOwnerController->PlayerCameraManager : nullptr };
	if (!ensureAlways(IsValid(CameraManager)))
	{
		return;
	}

	CameraManager->StartCameraShake(SkillCameraShakeBaseClass, 1.f);
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

	UKismetSystemLibrary::DrawDebugSphere(this, DetectSphereLocation, SkillDetectingSphereRadius, 16, FColor::Red, 10.f);

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

	ElapsedTime += 1.f;

	float MinTime{};
	float MaxTime{};
	SkillCameraCurveFloat->GetTimeRange(MinTime, MaxTime);

	if (ElapsedTime > MaxTime)
	{
		World->GetTimerManager().ClearTimer(SkillCameraCurveTimerHandle);
		return;
	}

	SkillOwnerCharacter->SetSpringArmZOffset(SkillCameraCurveFloat->GetFloatValue(ElapsedTime));
}
