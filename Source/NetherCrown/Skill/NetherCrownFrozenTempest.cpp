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

	CachedSkillCameraZoomCurveVector = SkillCameraZoomCurveVectorSoft.LoadSynchronous();

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
	NetherCrownKnightAnimInstance->GetOnHitFrozenTempestSkill().AddUObject(this, &ThisClass::HandleOnHitFrozenTempestSkill);}

void UNetherCrownFrozenTempest::ExecuteSkillGameplay()
{
	Super::ExecuteSkillGameplay();
}

void UNetherCrownFrozenTempest::PlaySkillCosmetics()
{
	Super::PlaySkillCosmetics();

	StartSkillCameraZoomCurveTimer();
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

	if (!SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	TArray<ANetherCrownEnemy*> DetectedEnemies{ GetSkillDetectedTargets() };
	//@TODO : 나온 Enemies들로 Material 적용, 움직임 막기, 애니메이션 stop 진행
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
