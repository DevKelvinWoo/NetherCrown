// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownEnemyRangedBasicAttackTask.h"

#include "NetherCrown/Components/NetherCrownEnemyRangedBasicAttackComponent.h"
#include "NetherCrown/Enemy/NetherCrownBossEnemy.h"
#include "NetherCrown/Enemy/AIController/NetherCrownEnemyAIController.h"

UNetherCrownEnemyRangedBasicAttackTask::UNetherCrownEnemyRangedBasicAttackTask()
{
	NodeName = TEXT("EnemyRangedBasicAttackTask");
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

	EnemyRangedBasicAttackComponent->GetOnEnemyRangedBasicAttackFinished().AddUObject(this, &ThisClass::HandleRangedBasicAttackFinished);

	CachedOwnerCompWeak = MakeWeakObjectPtr(&OwnerComp);

	EnemyRangedBasicAttackComponent->RequestEnemyRangedAttack();

	return EBTNodeResult::InProgress;
}

void UNetherCrownEnemyRangedBasicAttackTask::HandleRangedBasicAttackFinished()
{
	UBehaviorTreeComponent* CachedOwnerComp{ CachedOwnerCompWeak.Get() };
	if (!ensureAlways(IsValid(CachedOwnerComp)))
	{
		CachedOwnerCompWeak.Reset();
		return;
	}

	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}
