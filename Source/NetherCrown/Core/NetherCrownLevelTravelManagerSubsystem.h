// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "NetherCrown/Data/NetherCrownLevelTravelDataAsset.h"
#include "NetherCrownLevelTravelManagerSubsystem.generated.h"

UCLASS()
class NETHERCROWN_API UNetherCrownLevelTravelManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	const UNetherCrownLevelTravelDataAsset* GetLevelTravelData(const FGameplayTag& LevelTag) const;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	void CacheLevelTravelDT();

	UPROPERTY(Transient)
	TObjectPtr<UDataTable> CachedLevelTravelDT{};
};
