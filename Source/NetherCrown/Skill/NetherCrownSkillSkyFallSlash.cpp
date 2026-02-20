// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownSkillSkyFallSlash.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NetherCrown/Character/NetherCrownPlayerController.h"
#include "NetherCrown/Character/AnimInstance/NetherCrownKnightAnimInstance.h"
#include "NetherCrown/Components/NetherCrownControlPPComponent.h"
#include "NetherCrown/Components/NetherCrownCrowdControlComponent.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Settings/NetherCrownDefaultSettings.h"
#include "NetherCrown/Util/NetherCrownCollisionChannels.h"

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

	NetherCrownKnightAnimInstance->GetOnHitSkyFallSlashSkill().RemoveAll(this);
	NetherCrownKnightAnimInstance->GetOnHitSkyFallSlashSkill().AddUObject(this, &ThisClass::HandleOnHitSkyFallSlashSkill);

	CreateArmMaterialInstanceDynamic();

	if (!SkillOwnerCharacter->HasAuthority())
	{
		CachedSkillCameraCurveFloat = SkillCameraCurveFloatSoft.LoadSynchronous();
		CachedSkillArmMaterialCurveFloat = SkillArmMaterialCurveFloatSoft.LoadSynchronous();

		BindTimelineFunctions();
	}
}

void UNetherCrownSkillSkyFallSlash::ExecuteSkillGameplay()
{
	Super::ExecuteSkillGameplay();

	//@NOTE : Only gameplay logic (server)
}

void UNetherCrownSkillSkyFallSlash::PlaySkillCosmetics()
{
	Super::PlaySkillCosmetics();

	//@NOTE : Only cometics logic (all client)

	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	if (SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	ApplyPostProcess(ENetherCrownPPType::Charging, 1.0f);

	StartSetSpringArmZOffsetTimeline();
	StartSetArmMaterialParamTimeline();
}

void UNetherCrownSkillSkyFallSlash::TickFloatTimeline(float DeltaTime)
{
	Super::TickFloatTimeline(DeltaTime);

	if (SpringArmZOffsetFloatTimeline.IsPlaying())
	{
		SpringArmZOffsetFloatTimeline.TickTimeline(DeltaTime);
	}

	if (ArmMaterialFloatTimeline.IsPlaying())
	{
		ArmMaterialFloatTimeline.TickTimeline(DeltaTime);
	}
}

void UNetherCrownSkillSkyFallSlash::StartSetSpringArmZOffsetTimeline()
{
	SpringArmZOffsetFloatTimeline.PlayFromStart();
}

void UNetherCrownSkillSkyFallSlash::StartSetArmMaterialParamTimeline()
{
	ArmMaterialFloatTimeline.PlayFromStart();
}

void UNetherCrownSkillSkyFallSlash::BindTimelineFunctions()
{
	FOnTimelineFloat SpringArmZOffsetProgressFunc{};
	SpringArmZOffsetProgressFunc.BindUFunction(this, FName("SetSpringArmZOffsetByFloatTimeline"));
	SpringArmZOffsetFloatTimeline.AddInterpFloat(CachedSkillCameraCurveFloat, SpringArmZOffsetProgressFunc);

	FOnTimelineFloat ArmMaterialFloatProgressFunc{};
	ArmMaterialFloatProgressFunc.BindUFunction(this, FName("SetArmMaterialParamByFloatTimeline"));
	SpringArmZOffsetFloatTimeline.AddInterpFloat(CachedSkillArmMaterialCurveFloat, ArmMaterialFloatProgressFunc);
}

void UNetherCrownSkillSkyFallSlash::SetArmMaterialParamByFloatTimeline(float FloatCurveValue)
{
	const UNetherCrownDefaultSettings* DefaultSettings{ GetDefault<UNetherCrownDefaultSettings>() };
	check(DefaultSettings);

	ArmMaterialInstanceDynamic->SetScalarParameterValue(DefaultSettings->SkyFallSlashArmMaterialParam, FloatCurveValue);
}

void UNetherCrownSkillSkyFallSlash::HandleOnHitSkyFallSlashSkill()
{
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

	const TArray<AActor*> DetectedActors{ GetSkillDetectedTargets() };
	if (DetectedActors.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleOnHitSkyFallSlashSkill - DetectedActors is empty"));
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
				CrowdControlComponent->KnockBack(DetectedEnemy->GetActorForwardVector() * -SkillKnockBackDistance);
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

	const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes{ UEngineTypes::ConvertToObjectType(ECC_Enemy) };
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

void UNetherCrownSkillSkyFallSlash::CreateArmMaterialInstanceDynamic()
{
	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	USkeletalMeshComponent* SkeletalMeshComponent{ SkillOwnerCharacter->GetMesh() };
	constexpr int32 ArmMaterialIndex{ 1 };

	ArmMaterialInstanceDynamic = SkeletalMeshComponent ? SkeletalMeshComponent->CreateDynamicMaterialInstance(ArmMaterialIndex) : nullptr;
	ensureAlways(IsValid(ArmMaterialInstanceDynamic));
}

void UNetherCrownSkillSkyFallSlash::SetSpringArmZOffsetByFloatTimeline(float FloatCurveValue)
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	SkillOwnerCharacter->SetSpringArmZOffset(FloatCurveValue);
}
