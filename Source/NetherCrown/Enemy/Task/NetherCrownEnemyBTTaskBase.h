// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Enemy/AIController/NetherCrownEnemyAIController.h"
#include "NetherCrownEnemyBTTaskBase.generated.h"

class UBlackboardComponent;

UCLASS(Abstract)
class NETHERCROWN_API UNetherCrownEnemyBTTaskBase : public UBTTaskNode
{
	GENERATED_BODY()

protected:
	ANetherCrownEnemyAIController* GetEnemyAIController(UBehaviorTreeComponent& OwnerComp) const;
	UBlackboardComponent* GetBlackboardComponent(UBehaviorTreeComponent& OwnerComp) const;

	template <typename TEnemy = ANetherCrownEnemy>
	TEnemy* GetControlledEnemy(UBehaviorTreeComponent& OwnerComp, const bool bRequireAuthority = true) const
	{
		const ANetherCrownEnemyAIController* EnemyAIController{ GetEnemyAIController(OwnerComp) };
		if (!IsValid(EnemyAIController))
		{
			return nullptr;
		}

		TEnemy* ControlledEnemy{ Cast<TEnemy>(EnemyAIController->GetPawn()) };
		if (!ensureAlways(IsValid(ControlledEnemy)))
		{
			return nullptr;
		}

		if (bRequireAuthority && !ControlledEnemy->HasAuthority())
		{
			return nullptr;
		}

		return ControlledEnemy;
	}
};
