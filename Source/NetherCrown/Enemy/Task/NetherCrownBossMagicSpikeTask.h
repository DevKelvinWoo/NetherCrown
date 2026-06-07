// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownEnemyBTTaskBase.h"
#include "NetherCrownBossMagicSpikeTask.generated.h"

class UNetherCrownEnemySkillObject;

UCLASS()
class NETHERCROWN_API UNetherCrownBossMagicSpikeTask : public UNetherCrownEnemyBTTaskBase
{
	GENERATED_BODY()

public:
	UNetherCrownBossMagicSpikeTask();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void HandleOnMagicSpikeSkillFinished();
	void ResetTaskState();

	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerCompWeak{};
	TWeakObjectPtr<UNetherCrownEnemySkillObject> CachedMagicSpikeSkillObjectWeak{};
};
