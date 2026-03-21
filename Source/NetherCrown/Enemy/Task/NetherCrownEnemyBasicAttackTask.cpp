// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyBasicAttackTask.h"

#include "AIController.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Enemy/AIController/NetherCrownEnemyAIController.h"
#include "NetherCrown/Enemy/Components/NetherCrownEnemyBasicAttackComponent.h"

UNetherCrownEnemyBasicAttackTask::UNetherCrownEnemyBasicAttackTask()
{
	NodeName = TEXT("Enemy Basic Attack");
}

EBTNodeResult::Type UNetherCrownEnemyBasicAttackTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	const ANetherCrownEnemyAIController* EnemyAIController{ Cast<ANetherCrownEnemyAIController>(OwnerComp.GetAIOwner()) };
	if (!ensureAlways(IsValid(EnemyAIController)))
	{
		return EBTNodeResult::Failed;
	}

	const ANetherCrownEnemy* ControlledEnemy{ Cast<ANetherCrownEnemy>(EnemyAIController->GetPawn()) };
	if (!ensureAlways(IsValid(ControlledEnemy)) || !ControlledEnemy->HasAuthority())
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
