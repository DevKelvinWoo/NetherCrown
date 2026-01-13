// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownFrozenTempest.h"

#include "Curves/CurveVector.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NetherCrown/Character/NetherCrownPlayerController.h"
#include "NetherCrown/Character/AnimInstance/NetherCrownKnightAnimInstance.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"

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
	PlayChargeCameraShake();
}

void UNetherCrownFrozenTempest::StartSkillCameraZoomCurveTimer()
{
	const UWorld* World{ GetWorld() };
	check(World);

	SkillCameraZoomCurveElapsedTime = 0.f;

	World->GetTimerManager().SetTimer(SkillCameraZoomCurveTimerHandle, this, &ThisClass::ApplySkillCameraZoomCurveFloat, 0.01f, true, 0.f);
}

void UNetherCrownFrozenTempest::ApplySkillCameraZoomCurveFloat()
{
	const UWorld* World{ GetWorld() };
	check(World);

	if (!ensureAlways(IsValid(CachedSkillCameraZoomCurveVector)))
	{
		World->GetTimerManager().ClearTimer(SkillCameraZoomCurveTimerHandle);
		return;
	}

	SkillCameraZoomCurveElapsedTime += 0.01f;

	float Min{};
	float Max{};
	CachedSkillCameraZoomCurveVector->GetTimeRange(Min, Max);

	if (SkillCameraZoomCurveElapsedTime > Max)
	{
		World->GetTimerManager().ClearTimer(SkillCameraZoomCurveTimerHandle);
		return;
	}

	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		World->GetTimerManager().ClearTimer(SkillCameraZoomCurveTimerHandle);
		return;
	}

	const FVector& SkillCameraZoomCurveFloatValue{ CachedSkillCameraZoomCurveVector->GetVectorValue(SkillCameraZoomCurveElapsedTime) };
	SkillOwnerCharacter->SetSpringArmLength(SkillCameraZoomCurveFloatValue.X);
	SkillOwnerCharacter->SetSpringArmZOffset(SkillCameraZoomCurveFloatValue.Z);
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

			CrowdControlComponent->Frozen(SkillDuration);
		}
	}
}

void UNetherCrownFrozenTempest::PlayChargeCameraShake()
{
	const ANetherCrownCharacter* SKillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SKillOwnerCharacter)))
	{
		return;
	}

	if (!SKillOwnerCharacter->IsLocallyControlled())
	{
		return;
	}

	const ANetherCrownPlayerController* PlayerController{ Cast<ANetherCrownPlayerController>(SKillOwnerCharacter->GetController()) };
	check(PlayerController);

	APlayerCameraManager* PlayerCameraManager{ PlayerController->PlayerCameraManager };
	check(PlayerCameraManager);

	PlayerCameraManager->StartCameraShake(SKillChargeCameraShakeBaseClass);
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
	const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes{ UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn) };
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
