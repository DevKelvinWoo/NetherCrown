// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "NetherCrownUpdateBossRangedAttackAvailableService.generated.h"

class UBlackboardComponent;

UCLASS()
class NETHERCROWN_API UNetherCrownUpdateBossRangedAttackAvailableService : public UBTService
{
	GENERATED_BODY()

public:
	UNetherCrownUpdateBossRangedAttackAvailableService();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	void SetIsTargetInRangedAttackRange(UBlackboardComponent* BlackboardComponent, const bool bIsTargetInRangedAttackRange) const;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorBlackboardKey{};

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsTargetInRangedAttackRangeBlackboardKey{};
};
