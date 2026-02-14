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
#include "NetherCrown/Util/NetherCrownCurveTimerUtil.h"

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

	StartSkillCameraZoomCurveTimer();
	StartCharacterOverlayMaterialStartTimer();
	PlayChargeCameraShake();
}

void UNetherCrownFrozenTempest::StartSkillCameraZoomCurveTimer()
{
	const UWorld* World{ GetWorld() };
	check(World);

	SkillCameraZoomCurveElapsedTime = 0.f;

	World->GetTimerManager().SetTimer(SkillCameraZoomCurveTimerHandle, this, &ThisClass::ApplySkillCameraZoomCurveVector, 0.01f, true, 0.f);
}

void UNetherCrownFrozenTempest::ApplySkillCameraZoomCurveVector()
{
	FNetherCrownCurveTimerData CurveTimerData{};
	CurveTimerData.WorldContextObject = this;
	CurveTimerData.Curve = CachedSkillCameraZoomCurveVector;
	CurveTimerData.TimerHandle = &SkillCameraZoomCurveTimerHandle;
	CurveTimerData.CurveElapsedTime = &SkillCameraZoomCurveElapsedTime;
	CurveTimerData.CurveElapsedTimeOffset = 0.01f;
	CurveTimerData.CallBack = [WeakThis = MakeWeakObjectPtr(this)]() { WeakThis->SetSkillCameraSpringArmValues(); };

	FNetherCrownCurveTimerUtil::ExecuteLoopTimerCallbackByCurve(CurveTimerData);
}

void UNetherCrownFrozenTempest::SetSkillCameraSpringArmValues()
{
	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	const FVector& SkillCameraZoomCurveFloatValue{ CachedSkillCameraZoomCurveVector->GetVectorValue(SkillCameraZoomCurveElapsedTime) };
	SkillOwnerCharacter->SetSpringArmLength(SkillCameraZoomCurveFloatValue.X);
	SkillOwnerCharacter->SetSpringArmZOffset(SkillCameraZoomCurveFloatValue.Z);
}

void UNetherCrownFrozenTempest::StartCharacterOverlayMaterialStartTimer()
{
	const UWorld* World{ GetWorld() };
	check(World);

	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	CachedDynamicFrozenTempestMaterial = UMaterialInstanceDynamic::Create(CachedFrozenTempestTargetOverlayMaterial, SkillOwnerCharacter);
	CharacterOverlayMaterialElapsedTime = 0.f;

	World->GetTimerManager().SetTimer(CharacterOverlayMaterialStartTimerHandle, this, &ThisClass::ApplyCharacterOverlayStartMaterial, 0.01f, true, 0.f);
}

void UNetherCrownFrozenTempest::ApplyCharacterOverlayStartMaterial()
{
	FNetherCrownCurveTimerData CurveTimerData{};
	CurveTimerData.WorldContextObject = this;
	CurveTimerData.Curve = CachedCharacterOverlayMaterialStartCurveFloat;
	CurveTimerData.TimerHandle = &CharacterOverlayMaterialStartTimerHandle;
	CurveTimerData.CurveElapsedTime = &CharacterOverlayMaterialElapsedTime;
	CurveTimerData.CurveElapsedTimeOffset = 0.01f;
	CurveTimerData.CallBack = [WeakThis = MakeWeakObjectPtr(this)]() { WeakThis->SetStartCharacterOverlayMaterialScalarParam(); };

	FNetherCrownCurveTimerUtil::ExecuteLoopTimerCallbackByCurve(CurveTimerData);
}

void UNetherCrownFrozenTempest::SetStartCharacterOverlayMaterialScalarParam()
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

	const float CharacterOverlayMaterialAlpha{ CachedCharacterOverlayMaterialStartCurveFloat->GetFloatValue(CharacterOverlayMaterialElapsedTime) };
	CachedDynamicFrozenTempestMaterial->SetScalarParameterValue(DefaultSettings->FrozenTempestTargetMaterialParam, CharacterOverlayMaterialAlpha);
}

void UNetherCrownFrozenTempest::StartCharacterOverlayMaterialEndTimer()
{
	const UWorld* World{ GetWorld() };
	check(World);

	CharacterOverlayMaterialElapsedTime = 0.f;

	World->GetTimerManager().SetTimer(CharacterOverlayMaterialEndTimerHandle, this, &ThisClass::ApplyCharacterOverlayEndMaterial, 0.01f, true, 0.f);
}

void UNetherCrownFrozenTempest::ApplyCharacterOverlayEndMaterial()
{
	FNetherCrownCurveTimerData CurveTimerData{};
	CurveTimerData.WorldContextObject = this;
	CurveTimerData.Curve = CachedCharacterOverlayMaterialEndCurveFloat;
	CurveTimerData.TimerHandle = &CharacterOverlayMaterialEndTimerHandle;
	CurveTimerData.CurveElapsedTime = &CharacterOverlayMaterialElapsedTime;
	CurveTimerData.CurveElapsedTimeOffset = 0.01f;
	CurveTimerData.CallBack = [WeakThis = MakeWeakObjectPtr(this)]() { WeakThis->SetEndCharacterOverlayMaterialScalarParam(); };

	FNetherCrownCurveTimerUtil::ExecuteLoopTimerCallbackByCurve(CurveTimerData);
}

void UNetherCrownFrozenTempest::SetEndCharacterOverlayMaterialScalarParam()
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

	const float CharacterOverlayMaterialAlpha{ CachedCharacterOverlayMaterialEndCurveFloat->GetFloatValue(CharacterOverlayMaterialElapsedTime) };
	DynamicOverlayMaterial->SetScalarParameterValue(DefaultSettings->FrozenTempestTargetMaterialParam, CharacterOverlayMaterialAlpha);
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

		ControlPPComponent->ApplyPostProcess(ENetherCrownPPType::Frozen, SkillDuration);
	}

	if (!SkillOwnerCharacter->HasAuthority())
	{
		StartCharacterOverlayMaterialEndTimer();
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
			DetectedEnemy->ApplyCrowdControl(ENetherCrownCrowdControlType::FROZEN, SkillDuration);
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
