// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownEnemyBTTaskBase.h"
#include "NetherCrownEnemyBasicAttackTask.generated.h"

UCLASS()
class NETHERCROWN_API UNetherCrownEnemyBasicAttackTask : public UNetherCrownEnemyBTTaskBase
{
	GENERATED_BODY()

public:
	UNetherCrownEnemyBasicAttackTask();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
