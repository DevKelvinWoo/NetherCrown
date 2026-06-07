// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownEnemyBTServiceBase.h"
#include "NetherCrownCheckCanChaseService.generated.h"

UCLASS()
class NETHERCROWN_API UNetherCrownCheckCanChaseService : public UNetherCrownEnemyBTServiceBase
{
	GENERATED_BODY()

public:
	UNetherCrownCheckCanChaseService();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, const float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector CanChaseBlackboardKey{};
};
