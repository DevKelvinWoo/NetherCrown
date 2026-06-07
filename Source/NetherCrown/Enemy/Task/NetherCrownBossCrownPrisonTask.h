// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownEnemyBTTaskBase.h"
#include "NetherCrownBossCrownPrisonTask.generated.h"

class UNetherCrownEnemySkillObject;

UCLASS()
class NETHERCROWN_API UNetherCrownBossCrownPrisonTask : public UNetherCrownEnemyBTTaskBase
{
	GENERATED_BODY()

public:
	UNetherCrownBossCrownPrisonTask();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void HandleOnCrownPrisonSkillFinished();
	void ResetTaskState();

	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerCompWeak{};
	TWeakObjectPtr<UNetherCrownEnemySkillObject> CachedCrownPrisonSkillObjectWeak{};
};
