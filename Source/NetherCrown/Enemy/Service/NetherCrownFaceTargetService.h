// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "NetherCrownFaceTargetService.generated.h"

UCLASS()
class NETHERCROWN_API UNetherCrownFaceTargetService : public UBTService
{
	GENERATED_BODY()

public:
	UNetherCrownFaceTargetService();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, const float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorBlackboardKey{};

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BlockFaceTargetBlackboardKey{};

	UPROPERTY(EditAnywhere, Category = "Rotation")
	float RotationInterpSpeed{ 10.0f };
};
