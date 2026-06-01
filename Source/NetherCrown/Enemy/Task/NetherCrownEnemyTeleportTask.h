// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NetherCrownEnemyTeleportTask.generated.h"

class UNetherCrownEnemySkillObject;

UCLASS()
class NETHERCROWN_API UNetherCrownEnemyTeleportTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UNetherCrownEnemyTeleportTask();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void HandleOnTeleportSkillFinished();
	void ResetTaskState();

	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerCompWeak{};
	TWeakObjectPtr<UNetherCrownEnemySkillObject> CachedTeleportSkillObjectWeak{};
};
