// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownFrozenTempest.h"

#include "Curves/CurveVector.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NetherCrown/Character/NetherCrownPlayerController.h"
#include "NetherCrown/Character/AnimInstance/NetherCrownKnightAnimInstance.h"
#include "NetherCrown/Components/NetherCrownControlPPComponent.h"
#include "NetherCrown/Components/NetherCrownCrowdControlComponent.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Settings/NetherCrownDefaultSettings.h"
#include "NetherCrown/Util/NetherCrownCollisionChannels.h"

void UNetherCrownFrozenTempest::InitSkillObject()
{
	Super::InitSkillObject();

	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
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

	NetherCrownKnightAnimInstance->GetOnHitFrozenTempestSkill().RemoveAll(this);
	NetherCrownKnightAnimInstance->GetOnHitFrozenTempestSkill().AddUObject(this, &ThisClass::HandleOnHitFrozenTempestSkill);

	if (!SkillOwnerCharacter->HasAuthority())
	{
		CachedSkillCameraZoomCurveVector = SkillCameraZoomCurveVectorSoft.LoadSynchronous();
		CachedFrozenTempestTargetOverlayMaterial = FrozenTempestTargetOverlayMaterialSoft.LoadSynchronous();
		CachedCharacterOverlayMaterialStartCurveFloat = CharacterOverlayMaterialStartCurveFloatSoft.LoadSynchronous();
		CachedCharacterOverlayMaterialEndCurveFloat = CharacterOverlayMaterialEndCurveFloatSoft.LoadSynchronous();

		BindTimelineFunctions();
	}
}

void UNetherCrownFrozenTempest::ExecuteSkillGameplay()
{
	Super::ExecuteSkillGameplay();
}

void UNetherCrownFrozenTempest::PlaySkillCosmetics()
{
	Super::PlaySkillCosmetics();

	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	if (SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	ApplyPostProcess(ENetherCrownPPType::Charging, 2.5f);

	StartSetSkillCameraZoomTimeline();
	StartSetCharacterOverlayStartMaterialTimeline();
	PlayChargeCameraShake();
}

void UNetherCrownFrozenTempest::TickFloatTimeline(float DeltaTime)
{
	Super::TickFloatTimeline(DeltaTime);

	if (SkillCameraZoomVectorTimeline.IsPlaying())
	{
		SkillCameraZoomVectorTimeline.TickTimeline(DeltaTime);
	}

	if (CharacterOverlayStartMaterialFloatTimeline.IsPlaying())
	{
		CharacterOverlayStartMaterialFloatTimeline.TickTimeline(DeltaTime);
	}

	if (CharacterOverlayEndMaterialFloatTimeline.IsPlaying())
	{
		CharacterOverlayEndMaterialFloatTimeline.TickTimeline(DeltaTime);
	}
}

void UNetherCrownFrozenTempest::BindTimelineFunctions()
{
	FOnTimelineVector SkillCameraZoomProgressFunc{};
	SkillCameraZoomProgressFunc.BindUFunction(this, FName("SetSkillCameraZoomByVectorTimeline"));
	SkillCameraZoomVectorTimeline.AddInterpVector(CachedSkillCameraZoomCurveVector, SkillCameraZoomProgressFunc);

	FOnTimelineFloat CharacterOverlayStartMaterialProgressFunc{};
	CharacterOverlayStartMaterialProgressFunc.BindUFunction(this, FName("SetCharacterOverlayStartMaterialByFloatTimeline"));
	CharacterOverlayStartMaterialFloatTimeline.AddInterpFloat(CachedCharacterOverlayMaterialStartCurveFloat, CharacterOverlayStartMaterialProgressFunc);

	FOnTimelineFloat CharacterOverlayEndMaterialProgressFunc{};
	CharacterOverlayEndMaterialProgressFunc.BindUFunction(this, FName("SetCharacterOverlayEndMaterialByFloatTimeline"));
	CharacterOverlayEndMaterialFloatTimeline.AddInterpFloat(CachedCharacterOverlayMaterialEndCurveFloat, CharacterOverlayEndMaterialProgressFunc);
}

void UNetherCrownFrozenTempest::StartSetSkillCameraZoomTimeline()
{
	SkillCameraZoomVectorTimeline.PlayFromStart();
}

void UNetherCrownFrozenTempest::StartSetCharacterOverlayStartMaterialTimeline()
{
	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	CachedDynamicFrozenTempestMaterial = UMaterialInstanceDynamic::Create(CachedFrozenTempestTargetOverlayMaterial, SkillOwnerCharacter);

	CharacterOverlayStartMaterialFloatTimeline.PlayFromStart();
}

void UNetherCrownFrozenTempest::StartSetCharacterOverlayEndMaterialTimeline()
{
	CharacterOverlayEndMaterialFloatTimeline.PlayFromStart();
}

void UNetherCrownFrozenTempest::SetSkillCameraZoomByVectorTimeline(FVector VectorCurveValue)
{
	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	SkillOwnerCharacter->SetSpringArmLength(VectorCurveValue.X);
	SkillOwnerCharacter->SetSpringArmZOffset(VectorCurveValue.Z);
}

void UNetherCrownFrozenTempest::SetCharacterOverlayStartMaterialByFloatTimeline(float FloatCurveValue)
{
	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	if (!IsValid(CachedDynamicFrozenTempestMaterial))
	{
		UE_LOG(LogTemp, Warning, TEXT("CachedDynamicFrozenTempestMaterial::Create failed"));
		return;
	}

	USkeletalMeshComponent* SkillOwnerMeshComponent{ SkillOwnerCharacter->GetMesh() };
	check(SkillOwnerMeshComponent);

	SkillOwnerMeshComponent->SetOverlayMaterial(CachedDynamicFrozenTempestMaterial);

	const UNetherCrownDefaultSettings* DefaultSettings{ GetDefault<UNetherCrownDefaultSettings>() };
	check(DefaultSettings);

	CachedDynamicFrozenTempestMaterial->SetScalarParameterValue(DefaultSettings->FrozenTempestTargetMaterialParam, FloatCurveValue);
}

void UNetherCrownFrozenTempest::SetCharacterOverlayEndMaterialByFloatTimeline(float FloatCurveValue)
{
	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	USkeletalMeshComponent* SkillOwnerMeshComponent{ SkillOwnerCharacter->GetMesh() };
	check(SkillOwnerMeshComponent);

	UMaterialInstanceDynamic* DynamicOverlayMaterial = Cast<UMaterialInstanceDynamic>(SkillOwnerMeshComponent->GetOverlayMaterial());
	if (!IsValid(DynamicOverlayMaterial))
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyCharacterOverlayEndMaterial - Cannot cast dynamic material"));
		return;
	}

	SkillOwnerMeshComponent->SetOverlayMaterial(DynamicOverlayMaterial);

	const UNetherCrownDefaultSettings* DefaultSettings{ GetDefault<UNetherCrownDefaultSettings>() };
	check(DefaultSettings);

	DynamicOverlayMaterial->SetScalarParameterValue(DefaultSettings->FrozenTempestTargetMaterialParam, FloatCurveValue);
}

void UNetherCrownFrozenTempest::HandleOnHitFrozenTempestSkill()
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

		UNetherCrownControlPPComponent* ControlPPComponent{ SkillOwnerCharacter->GetControlPPComponent() };
		if (!ensureAlways(IsValid(ControlPPComponent)))
		{
			return;
		}

		ControlPPComponent->ApplyPostProcess(ENetherCrownPPType::Frozen, FrozenDuration);
	}

	if (!SkillOwnerCharacter->HasAuthority())
	{
		StartSetCharacterOverlayEndMaterialTimeline();
	}

	const TArray<ANetherCrownEnemy*> DetectedEnemies{ GetSkillDetectedTargets() };
	if (DetectedEnemies.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleOnHitFrozenTempestSkill - DetectedActors is empty"));
		return;
	}

	for (ANetherCrownEnemy* DetectedEnemy : DetectedEnemies)
	{
		if (SkillOwnerCharacter->HasAuthority())
		{
			ApplyCrowdControlToTarget(DetectedEnemy, ENetherCrownCrowdControlType::FROZEN, FrozenDuration);
			continue;
		}

		const UNetherCrownCrowdControlComponent* CrowdControlComponent{ DetectedEnemy->GetCrowdControlComponent() };
		if (IsValid(CrowdControlComponent))
		{
			if (IsValid(CachedFrozenTempestTargetOverlayMaterial))
			{
				USkeletalMeshComponent* DetectedEnemyMesh{ DetectedEnemy->GetMesh() };
				check(DetectedEnemyMesh);

				DetectedEnemyMesh->SetOverlayMaterial(CachedFrozenTempestTargetOverlayMaterial);
			}

			CrowdControlComponent->Frozen();
		}
	}
}

void UNetherCrownFrozenTempest::PlayChargeCameraShake()
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	if (!SkillOwnerCharacter->IsLocallyControlled())
	{
		return;
	}

	const ANetherCrownPlayerController* PlayerController{ Cast<ANetherCrownPlayerController>(SkillOwnerCharacter->GetController()) };
	check(PlayerController);

	APlayerCameraManager* PlayerCameraManager{ PlayerController->PlayerCameraManager };
	check(PlayerCameraManager);

	PlayerCameraManager->StartCameraShake(SkillChargeCameraShakeBaseClass);
}

const TArray<ANetherCrownEnemy*> UNetherCrownFrozenTempest::GetSkillDetectedTargets() const
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return {};
	}

	const FVector& SkillOwnerCharacterLocation{ SkillOwnerCharacter->GetActorLocation() };

	TArray<AActor*> OverlappedActors{};
	const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes{ UEngineTypes::ConvertToObjectType(ECC_Enemy) };
	const bool bDetectEnemy{ UKismetSystemLibrary::SphereOverlapActors(this, SkillOwnerCharacterLocation,
		SkillDetectingSphereRadius, ObjectTypes, ANetherCrownEnemy::StaticClass(), TArray<AActor*>(), OverlappedActors) };

#if 0
	UKismetSystemLibrary::DrawDebugSphere(this, SkillOwnerCharacterLocation, SkillDetectingSphereRadius, 16, FColor::Red, 10.f);
#endif

	if (!bDetectEnemy)
	{
		return {};
	}

	TArray<ANetherCrownEnemy*> DetectedEnemies{};
	for (AActor* OverlappedActor : OverlappedActors)
	{
		DetectedEnemies.Add(Cast<ANetherCrownEnemy>(OverlappedActor));
	}

	return DetectedEnemies;
}
