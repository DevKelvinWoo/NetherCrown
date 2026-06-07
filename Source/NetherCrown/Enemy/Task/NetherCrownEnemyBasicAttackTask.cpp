// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyBasicAttackTask.h"

#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Enemy/Components/NetherCrownEnemyBasicAttackComponent.h"

UNetherCrownEnemyBasicAttackTask::UNetherCrownEnemyBasicAttackTask()
{
	NodeName = TEXT("Enemy Basic Attack");
}

EBTNodeResult::Type UNetherCrownEnemyBasicAttackTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	const ANetherCrownEnemy* ControlledEnemy{ GetControlledEnemy(OwnerComp) };
	if (!IsValid(ControlledEnemy))
	{
		return EBTNodeResult::Failed;
	}

	UNetherCrownEnemyBasicAttackComponent* EnemyBasicAttackComponent{ ControlledEnemy->FindComponentByClass<UNetherCrownEnemyBasicAttackComponent>() };
	if (!ensureAlways(IsValid(EnemyBasicAttackComponent)))
	{
		return EBTNodeResult::Failed;
	}

	EnemyBasicAttackComponent->RequestEnemyAttack();

	return EBTNodeResult::Succeeded;
}
