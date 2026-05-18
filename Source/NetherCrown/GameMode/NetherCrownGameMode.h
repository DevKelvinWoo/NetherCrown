// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameplayTagContainer.h"
#include "NetherCrownGameMode.generated.h"

UCLASS()
class NETHERCROWN_API ANetherCrownGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	void TravelLevelByTag(const FGameplayTag& LevelTag);
};
