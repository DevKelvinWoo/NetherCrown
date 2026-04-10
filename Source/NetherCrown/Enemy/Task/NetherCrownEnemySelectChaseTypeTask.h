// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NetherCrownEnemySelectChaseTypeTask.generated.h"

UENUM(BlueprintType)
enum class ENetherCrownEnemyChaseType : uint8
{
	None,
	DashChase,
	RunChase
};

UCLASS()
class NETHERCROWN_API UNetherCrownEnemySelectChaseTypeTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UNetherCrownEnemySelectChaseTypeTask();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ChaseTypeBlackboardKey{};
};
