// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NetherCrownBossVoidPiercerTask.generated.h"

UCLASS()
class NETHERCROWN_API UNetherCrownBossVoidPiercerTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UNetherCrownBossVoidPiercerTask();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void HandleVoidPiercerFinished();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BlockFaceTargetBlackboardKey{};

	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerCompWeak{};
};
