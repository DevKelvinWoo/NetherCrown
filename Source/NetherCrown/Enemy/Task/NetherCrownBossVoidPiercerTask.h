// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownEnemyBTTaskBase.h"
#include "NetherCrownBossVoidPiercerTask.generated.h"

class UNetherCrownEnemySkillObject;

UCLASS()
class NETHERCROWN_API UNetherCrownBossVoidPiercerTask : public UNetherCrownEnemyBTTaskBase
{
	GENERATED_BODY()

public:
	UNetherCrownBossVoidPiercerTask();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void HandleVoidPiercerFinished();
	void ResetTaskState();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BlockFaceTargetBlackboardKey{};

	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerCompWeak{};
	TWeakObjectPtr<UNetherCrownEnemySkillObject> CachedVoidPiercerSkillObjectWeak{};
};
