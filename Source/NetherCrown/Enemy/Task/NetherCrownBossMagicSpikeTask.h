// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NetherCrownBossMagicSpikeTask.generated.h"

UCLASS()
class NETHERCROWN_API UNetherCrownBossMagicSpikeTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UNetherCrownBossMagicSpikeTask();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void HandleOnMagicSpikeSkillFinished();

	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerCompWeak{};
};
