// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownSkillEnemyDashAttack.h"

#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Enemy/Components/NetherCrownEnemyBasicAttackComponent.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

void UNetherCrownSkillEnemyDashAttack::InitEnemySkillObject()
{
	Super::InitEnemySkillObject();

	CacheEnemyDashAttackData();
}

void UNetherCrownSkillEnemyDashAttack::DetectTargetAndAttack()
{
	const ANetherCrownEnemy* SkillOwnerEnemy{ SkillOwnerEnemyWeak.IsValid() ? SkillOwnerEnemyWeak.Get() : nullptr };
	if (!ensureAlways(IsValid(SkillOwnerEnemy)) || !SkillOwnerEnemy->HasAuthority())
	{
		return;
	}

	UNetherCrownEnemyBasicAttackComponent* EnemyBasicAttackComponent{ SkillOwnerEnemy->GetEnemyBasicAttackComponent() };
	if (!ensureAlways(IsValid(EnemyBasicAttackComponent)))
	{
		return;
	}

	const UNetherCrownEnemyBasicAttackDataAsset* DashFollowUpBasicAttackDataAsset{ CachedEnemyDashAttackData.DashFollowUpBasicAttackDataAssetSoft.LoadSynchronous() };
	if (!ensureAlways(IsValid(DashFollowUpBasicAttackDataAsset)))
	{
		return;
	}

	EnemyBasicAttackComponent->RequestEnemyAttackByDA(DashFollowUpBasicAttackDataAsset);
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
	DetectTargetAndAttack();
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
		BroadcastDashAttackFinished();
		return;
	}

	const ANetherCrownCharacter* CurrentTargetCharacter{ SkillOwnerEnemy->GetCurrentTargetCharacter() };
	if (!ensureAlways(IsValid(CurrentTargetCharacter)))
	{
		BroadcastDashAttackFinished();
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
		BroadcastDashAttackFinished();
		return;
	}
	SkillOwnerEnemyMovementComponent->ApplyRootMotionSource(MoveToForceRootMotionSource);

	if (!SkillOwnerEnemy->HasAuthority())
	{
		return;
	}

	const UWorld* World{ SkillOwnerEnemy->GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		BroadcastDashAttackFinished();
		return;
	}

	World->GetTimerManager().ClearTimer(TimerHandle_DashAttackFinished);
	World->GetTimerManager().SetTimer(TimerHandle_DashAttackFinished, this, &ThisClass::BroadcastDashAttackFinished, CachedEnemyDashAttackData.DashAttackDuration, false);
}

void UNetherCrownSkillEnemyDashAttack::BroadcastDashAttackFinished()
{
	const ANetherCrownEnemy* SkillOwnerEnemy{ SkillOwnerEnemyWeak.IsValid() ? SkillOwnerEnemyWeak.Get() : nullptr };
	if (!IsValid(SkillOwnerEnemy) || !SkillOwnerEnemy->HasAuthority())
	{
		return;
	}

	if (const UWorld* World = SkillOwnerEnemy->GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimerHandle_DashAttackFinished);
	}

	OnEnemyDashAttackFinished.Broadcast();
}
