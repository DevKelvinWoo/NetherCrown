// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownEnemyBTTaskBase.h"
#include "NetherCrownEnemyRunningChaseTask.generated.h"

UCLASS()
class NETHERCROWN_API UNetherCrownEnemyRunningChaseTask : public UNetherCrownEnemyBTTaskBase
{
	GENERATED_BODY()

public:
	UNetherCrownEnemyRunningChaseTask();

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ChaseTypeBlackboardKey{};

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector NeedRunChaseBlackboardKey{};
};
