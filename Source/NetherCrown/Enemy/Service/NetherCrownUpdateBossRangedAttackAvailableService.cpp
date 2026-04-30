// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownUpdateBossRangedAttackAvailableService.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NetherCrown/Data/NetherCrownEnemyAITuningData.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Enemy/AIController/NetherCrownEnemyAIController.h"

UNetherCrownUpdateBossRangedAttackAvailableService::UNetherCrownUpdateBossRangedAttackAvailableService()
{
	NodeName = TEXT("Update Boss Ranged Attack Available");

	TargetActorBlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, TargetActorBlackboardKey), AActor::StaticClass());
	IsTargetInRangedAttackRangeBlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, IsTargetInRangedAttackRangeBlackboardKey));

	Interval = 0.1f;
	RandomDeviation = 0.0f;
}

void UNetherCrownUpdateBossRangedAttackAvailableService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const ANetherCrownEnemyAIController* EnemyAIController{ Cast<ANetherCrownEnemyAIController>(OwnerComp.GetAIOwner()) };
	if (!ensureAlways(IsValid(EnemyAIController)))
	{
		return;
	}

	const ANetherCrownEnemy* OwnerEnemy{ Cast<ANetherCrownEnemy>(EnemyAIController->GetPawn()) };
	if (!ensureAlways(IsValid(OwnerEnemy)) || !OwnerEnemy->HasAuthority())
	{
		return;
	}

	UBlackboardComponent* BlackboardComponent{ OwnerComp.GetBlackboardComponent() };
	if (!ensureAlways(IsValid(BlackboardComponent)))
	{
		return;
	}

	const AActor* TargetActor{ Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetActorBlackboardKey.SelectedKeyName)) };
	if (!IsValid(TargetActor))
	{
		SetIsTargetInRangedAttackRange(BlackboardComponent, false);
		return;
	}

	const double TargetDistance{ FVector::Dist2D(OwnerEnemy->GetActorLocation(), TargetActor->GetActorLocation()) };
	const FNetherCrownBossRangedCombatData& BossRangedCombatData{ EnemyAIController->GetBossRangedCombatData() };
	const bool bIsTargetInRangedAttackRange{ TargetDistance >= BossRangedCombatData.MinAttackDistance && TargetDistance <= BossRangedCombatData.MaxAttackDistance };

	SetIsTargetInRangedAttackRange(BlackboardComponent, bIsTargetInRangedAttackRange);
}

void UNetherCrownUpdateBossRangedAttackAvailableService::SetIsTargetInRangedAttackRange(UBlackboardComponent* BlackboardComponent, const bool bIsTargetInRangedAttackRange) const
{
	if (!ensureAlways(IsValid(BlackboardComponent)))
	{
		return;
	}

	BlackboardComponent->SetValueAsBool(IsTargetInRangedAttackRangeBlackboardKey.SelectedKeyName, bIsTargetInRangedAttackRange);
}
