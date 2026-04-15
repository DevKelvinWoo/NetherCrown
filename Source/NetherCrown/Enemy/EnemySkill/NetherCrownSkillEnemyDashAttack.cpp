// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownSkillEnemyDashAttack.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

void UNetherCrownSkillEnemyDashAttack::InitEnemySkillObject()
{
	Super::InitEnemySkillObject();

	CacheEnemyDashAttackData();
}

void UNetherCrownSkillEnemyDashAttack::CacheEnemyDashAttackData()
{
	const UNetherCrownEnemySkillDataAsset* EnemySkillDataAsset{ FNetherCrownUtilManager::GetEnemySkillDataAssetByGameplayTag(NetherCrownTags::Enemy_Skill_DashAttack) };
	const UNetherCrownEnemyDashAttackDataAsset* EnemyDashAttackDataAsset{ Cast<UNetherCrownEnemyDashAttackDataAsset>(EnemySkillDataAsset) };
	if (!ensureAlways(IsValid(EnemyDashAttackDataAsset)))
	{
		return;
	}

	CachedEnemyDashAttackData = EnemyDashAttackDataAsset->GetEnemyDashAttackData();
}

void UNetherCrownSkillEnemyDashAttack::PlayEnemySkillCosmetics()
{
	Super::PlayEnemySkillCosmetics();

	UE_LOG(LogTemp, Warning, TEXT("Play Enemy SkillCosmetics!!!"));
}

void UNetherCrownSkillEnemyDashAttack::ExecuteEnemySkillGameplay()
{
	Super::ExecuteEnemySkillGameplay();

	UE_LOG(LogTemp, Warning, TEXT("Execute Enemy SkillGameplay!!!"));

	Multicast_EnemyDashAttack();
}

void UNetherCrownSkillEnemyDashAttack::Multicast_EnemyDashAttack_Implementation()
{
	EnemyDashAttack();
}

void UNetherCrownSkillEnemyDashAttack::EnemyDashAttack()
{
	ANetherCrownEnemy* SkillOwnerEnemy{ SkillOwnerEnemyWeak.IsValid() ? SkillOwnerEnemyWeak.Get() : nullptr };
	if (!ensureAlways(IsValid(SkillOwnerEnemy)))
	{
		return;
	}

	const ANetherCrownCharacter* CurrentTargetCharacter{ SkillOwnerEnemy->GetCurrentTargetCharacter() };
	if (!ensureAlways(IsValid(CurrentTargetCharacter)))
	{
		return;
	}

	const FVector CurrentTargetLocation{ CurrentTargetCharacter->GetActorLocation() };
	const FVector SkillOwnerEnemyLocation{ SkillOwnerEnemy->GetActorLocation() };

	const FRotator LookAtRotation{ UKismetMathLibrary::FindLookAtRotation(SkillOwnerEnemyLocation, CurrentTargetLocation) };
	SkillOwnerEnemy->SetActorRotation(LookAtRotation);

	const FVector EnemyForwardVector{ SkillOwnerEnemy->GetActorForwardVector() };
	const FVector DashTargetLocation{ CurrentTargetLocation + (-EnemyForwardVector * CachedEnemyDashAttackData.DashOffset) };

	TSharedPtr<FRootMotionSource_MoveToForce> MoveToForceRootMotionSource{ MakeShared<FRootMotionSource_MoveToForce>() };
	MoveToForceRootMotionSource->InstanceName = TEXT("EnemyDashAttack");
	MoveToForceRootMotionSource->AccumulateMode = ERootMotionAccumulateMode::Override;
	MoveToForceRootMotionSource->Priority = 5;
	MoveToForceRootMotionSource->StartLocation = SkillOwnerEnemyLocation;
	MoveToForceRootMotionSource->TargetLocation = DashTargetLocation;
	MoveToForceRootMotionSource->Duration = CachedEnemyDashAttackData.DashDuration;
	MoveToForceRootMotionSource->bRestrictSpeedToExpected = true;

	MoveToForceRootMotionSource->FinishVelocityParams.Mode = ERootMotionFinishVelocityMode::SetVelocity;
	MoveToForceRootMotionSource->FinishVelocityParams.SetVelocity = FVector::ZeroVector;

	UCharacterMovementComponent* SkillOwnerEnemyMovementComponent{ SkillOwnerEnemy->GetCharacterMovement() };
	if (!ensureAlways(IsValid(SkillOwnerEnemyMovementComponent)))
	{
		return;
	}
	SkillOwnerEnemyMovementComponent->ApplyRootMotionSource(MoveToForceRootMotionSource);
}
