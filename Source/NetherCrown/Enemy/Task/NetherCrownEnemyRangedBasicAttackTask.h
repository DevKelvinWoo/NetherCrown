// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownEnemyBTTaskBase.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "NetherCrownEnemyRangedBasicAttackTask.generated.h"

class UNetherCrownEnemyRangedBasicAttackComponent;

UCLASS()
class NETHERCROWN_API UNetherCrownEnemyRangedBasicAttackTask : public UNetherCrownEnemyBTTaskBase
{
	GENERATED_BODY()

public:
	UNetherCrownEnemyRangedBasicAttackTask();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void HandleRangedBasicAttackFinished();
	void ResetTaskState();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ShouldRepositionBlackboardKey{};

	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerCompWeak{};
	TWeakObjectPtr<UNetherCrownEnemyRangedBasicAttackComponent> CachedEnemyRangedBasicAttackComponentWeak{};
};
