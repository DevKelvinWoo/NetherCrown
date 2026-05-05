// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "NetherCrownEnemySkillCoolDownCheckService.generated.h"

UCLASS()
class NETHERCROWN_API UNetherCrownEnemySkillCoolDownCheckService : public UBTService
{
	GENERATED_BODY()

public:
	UNetherCrownEnemySkillCoolDownCheckService();

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FGameplayTag SkillGameplayTag{};


};
