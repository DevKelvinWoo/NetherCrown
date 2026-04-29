// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "NetherCrownUpdateRangedBossMovementService.generated.h"

class UBlackboardComponent;

UCLASS()
class NETHERCROWN_API UNetherCrownUpdateRangedBossMovementService : public UBTService
{
	GENERATED_BODY()

public:
	UNetherCrownUpdateRangedBossMovementService();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	void ClearMovementState(UBlackboardComponent* BlackboardComponent) const;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorBlackboardKey{};

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetDistanceBlackboardKey{};

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector NeedEscapeBlackboardKey{};

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector NeedApproachBlackboardKey{};

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector NeedOrbitBlackboardKey{};

	UPROPERTY(EditAnywhere, Category = "RangedBoss")
	float MinAttackDistance{ 650.0f };

	UPROPERTY(EditAnywhere, Category = "RangedBoss")
	float MaxAttackDistance{ 1350.0f };
};
