// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownEnemyRangedBasicAttackTask.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NetherCrown/Components/NetherCrownEnemyRangedBasicAttackComponent.h"
#include "NetherCrown/Enemy/NetherCrownBossEnemy.h"
#include "NetherCrown/Enemy/AIController/NetherCrownEnemyAIController.h"

UNetherCrownEnemyRangedBasicAttackTask::UNetherCrownEnemyRangedBasicAttackTask()
{
	NodeName = TEXT("EnemyRangedBasicAttackTask");
	bCreateNodeInstance = true;

	ShouldRepositionBlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, ShouldRepositionBlackboardKey));
}

EBTNodeResult::Type UNetherCrownEnemyRangedBasicAttackTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	const ANetherCrownEnemyAIController* EnemyAIController{ Cast<ANetherCrownEnemyAIController>(OwnerComp.GetAIOwner()) };
	if (!ensureAlways(IsValid(EnemyAIController)))
	{
		return EBTNodeResult::Failed;
	}

	const ANetherCrownBossEnemy* BossEnemy{ EnemyAIController->GetPawn() };
	if (!ensureAlways(IsValid(BossEnemy)) || !BossEnemy->HasAuthority())
	{
		return EBTNodeResult::Failed;
	}

	UNetherCrownEnemyRangedBasicAttackComponent* EnemyRangedBasicAttackComponent{ BossEnemy->GetRangedBasicAttackComponent() };
	if (!ensureAlways(IsValid(EnemyRangedBasicAttackComponent)))
	{
		return EBTNodeResult::Failed;
	}

	CachedEnemyRangedBasicAttackComponentWeak = MakeWeakObjectPtr(EnemyRangedBasicAttackComponent);
	EnemyRangedBasicAttackComponent->GetOnEnemyRangedBasicAttackFinished().RemoveAll(this);
	EnemyRangedBasicAttackComponent->GetOnEnemyRangedBasicAttackFinished().AddUObject(this, &ThisClass::HandleRangedBasicAttackFinished);

	CachedOwnerCompWeak = MakeWeakObjectPtr(&OwnerComp);

	if (!EnemyRangedBasicAttackComponent->RequestEnemyRangedAttack())
	{
		ResetTaskState();
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UNetherCrownEnemyRangedBasicAttackTask::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);

	ResetTaskState();

	return EBTNodeResult::Aborted;
}

void UNetherCrownEnemyRangedBasicAttackTask::HandleRangedBasicAttackFinished()
{
	UBehaviorTreeComponent* CachedOwnerComp{ CachedOwnerCompWeak.Get() };
	if (!ensureAlways(IsValid(CachedOwnerComp)))
	{
		ResetTaskState();
		return;
	}

	UBlackboardComponent* BlackboardComponent{ CachedOwnerComp->GetBlackboardComponent() };
	if (ensureAlways(IsValid(BlackboardComponent)))
	{
		BlackboardComponent->SetValueAsBool(ShouldRepositionBlackboardKey.SelectedKeyName, true);
	}

	ResetTaskState();
	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}

void UNetherCrownEnemyRangedBasicAttackTask::ResetTaskState()
{
	if (UNetherCrownEnemyRangedBasicAttackComponent* EnemyRangedBasicAttackComponent{ CachedEnemyRangedBasicAttackComponentWeak.Get() })
	{
		EnemyRangedBasicAttackComponent->GetOnEnemyRangedBasicAttackFinished().RemoveAll(this);
	}

	CachedEnemyRangedBasicAttackComponentWeak.Reset();
	CachedOwnerCompWeak.Reset();
}
