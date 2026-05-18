// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NetherCrownLevelTravelPersistenceSubsystem.generated.h"

UCLASS()
class NETHERCROWN_API UNetherCrownLevelTravelPersistenceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void BeginLevelTravelPersistence();
	void RestoreLevelTravelPersistence();

private:
	bool bHasPendingPersistence{ false };
};
