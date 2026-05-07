// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NetherCrownBossCrownPrisonTask.generated.h"

UCLASS()
class NETHERCROWN_API UNetherCrownBossCrownPrisonTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UNetherCrownBossCrownPrisonTask();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
