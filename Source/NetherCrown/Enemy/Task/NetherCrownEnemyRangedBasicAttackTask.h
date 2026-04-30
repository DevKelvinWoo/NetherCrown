// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "NetherCrownEnemyRangedBasicAttackTask.generated.h"

UCLASS()
class NETHERCROWN_API UNetherCrownEnemyRangedBasicAttackTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UNetherCrownEnemyRangedBasicAttackTask();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void HandleRangedBasicAttackFinished();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ShouldRepositionBlackboardKey{};

	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerCompWeak{};
};
