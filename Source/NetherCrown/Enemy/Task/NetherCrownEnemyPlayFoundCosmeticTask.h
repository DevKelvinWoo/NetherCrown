// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NetherCrownEnemyPlayFoundCosmeticTask.generated.h"

UCLASS()
class NETHERCROWN_API UNetherCrownEnemyPlayFoundCosmeticTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UNetherCrownEnemyPlayFoundCosmeticTask();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector NeedFoundReactionBlackboardKey{};
};
