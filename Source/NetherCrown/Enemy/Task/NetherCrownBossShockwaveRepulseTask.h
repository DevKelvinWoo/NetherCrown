// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NetherCrownBossShockwaveRepulseTask.generated.h"

UCLASS()
class NETHERCROWN_API UNetherCrownBossShockwaveRepulseTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UNetherCrownBossShockwaveRepulseTask();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void HandleOnShockwaveRepulseSkillFinished();

	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerCompWeak{};
};
