// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "NetherCrownSetBlackboardBoolTask.generated.h"

UCLASS()
class NETHERCROWN_API UNetherCrownSetBlackboardBoolTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UNetherCrownSetBlackboardBoolTask();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BlackboardKey{};

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	bool bValue{ false };
};
