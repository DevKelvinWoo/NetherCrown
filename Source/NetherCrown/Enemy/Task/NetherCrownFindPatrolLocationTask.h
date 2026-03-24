// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "NetherCrownFindPatrolLocationTask.generated.h"

UCLASS()
class NETHERCROWN_API UNetherCrownFindPatrolLocationTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UNetherCrownFindPatrolLocationTask();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PatrolLocationBlackboardKey{};
};
