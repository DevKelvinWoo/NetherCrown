// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyBTTaskBase.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NetherCrown/Enemy/AIController/NetherCrownEnemyAIController.h"

ANetherCrownEnemyAIController* UNetherCrownEnemyBTTaskBase::GetEnemyAIController(UBehaviorTreeComponent& OwnerComp) const
{
	ANetherCrownEnemyAIController* EnemyAIController{ Cast<ANetherCrownEnemyAIController>(OwnerComp.GetAIOwner()) };
	if (!ensureAlways(IsValid(EnemyAIController)))
	{
		return nullptr;
	}

	return EnemyAIController;
}

UBlackboardComponent* UNetherCrownEnemyBTTaskBase::GetBlackboardComponent(UBehaviorTreeComponent& OwnerComp) const
{
	UBlackboardComponent* BlackboardComponent{ OwnerComp.GetBlackboardComponent() };
	if (!ensureAlways(IsValid(BlackboardComponent)))
	{
		return nullptr;
	}

	return BlackboardComponent;
}
