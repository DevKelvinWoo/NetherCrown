// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "NetherCrownLevelTravelDataAsset.generated.h"

USTRUCT()
struct FNetherCrownLevelTravelData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "LevelTravelData")
	FGameplayTag LevelTravelTag{};

	UPROPERTY(EditDefaultsOnly, Category = "LevelTravelData")
	FSoftObjectPath LevelPathSoft{};
};

USTRUCT()
struct FNetherCrownLevelTravelDataTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	const UNetherCrownLevelTravelDataAsset* GetLevelTravelDataAsset() const { return LevelTravelDataAsset.Get(); }

private:
	UPROPERTY(EditAnywhere, Category = "Tag")
	TObjectPtr<UNetherCrownLevelTravelDataAsset> LevelTravelDataAsset{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownLevelTravelDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownLevelTravelData& GetLevelTravelData() const { return LevelTravelData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "LevelTravelData")
	FNetherCrownLevelTravelData LevelTravelData{};
};
