// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NetherCrownEnemyDashAttackTask.generated.h"

class UBehaviorTreeComponent;
class UNetherCrownSkillEnemyDashAttack;

UCLASS()
class NETHERCROWN_API UNetherCrownEnemyDashAttackTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UNetherCrownEnemyDashAttackTask();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void HandleDashAttackFinished();
	void ResetTaskState();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector NeedDashAttackBlackboardKey{};

	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerCompWeak{};
	TWeakObjectPtr<UNetherCrownSkillEnemyDashAttack> CachedDashAttackSkillWeak{};
	FDelegateHandle DashAttackFinishedDelegateHandle{};
};
