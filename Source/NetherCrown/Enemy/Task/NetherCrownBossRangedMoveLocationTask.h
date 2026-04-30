// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "NetherCrownBossRangedMoveLocationTask.generated.h"

UENUM()
enum class ENetherCrownBossRangedMoveType : uint8
{
	Auto,
	Escape,
	Approach,
	Orbit
};

UCLASS()
class NETHERCROWN_API UNetherCrownBossRangedMoveLocationTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UNetherCrownBossRangedMoveLocationTask();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	ENetherCrownBossRangedMoveType ResolveMoveType(const FVector& OwnerLocation, const FVector& TargetLocation, const float MinAttackDistance, const float MaxAttackDistance) const;
	FVector GetDesiredDirectionFromTarget(const FVector& OwnerLocation, const FVector& TargetLocation, const ENetherCrownBossRangedMoveType ResolvedMoveType, const float OrbitAngleDegrees) const;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorBlackboardKey{};

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector MoveLocationBlackboardKey{};

	UPROPERTY(EditAnywhere, Category = "RangedBoss")
	ENetherCrownBossRangedMoveType MoveType{ ENetherCrownBossRangedMoveType::Auto };
};
