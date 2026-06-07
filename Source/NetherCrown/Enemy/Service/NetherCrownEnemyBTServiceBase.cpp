// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyBTServiceBase.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NetherCrown/Enemy/AIController/NetherCrownEnemyAIController.h"

ANetherCrownEnemyAIController* UNetherCrownEnemyBTServiceBase::GetEnemyAIController(UBehaviorTreeComponent& OwnerComp) const
{
	ANetherCrownEnemyAIController* EnemyAIController{ Cast<ANetherCrownEnemyAIController>(OwnerComp.GetAIOwner()) };
	if (!ensureAlways(IsValid(EnemyAIController)))
	{
		return nullptr;
	}

	return EnemyAIController;
}

UBlackboardComponent* UNetherCrownEnemyBTServiceBase::GetBlackboardComponent(UBehaviorTreeComponent& OwnerComp) const
{
	UBlackboardComponent* BlackboardComponent{ OwnerComp.GetBlackboardComponent() };
	if (!ensureAlways(IsValid(BlackboardComponent)))
	{
		return nullptr;
	}

	return BlackboardComponent;
}
