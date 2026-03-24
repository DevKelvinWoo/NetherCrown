// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NetherCrown/Components/NetherCrownCrowdControlTypes.h"
#include "NetherCrownCrowdControlCosmeticData.generated.h"

class UAnimMontage;

USTRUCT()
struct FNetherCrownCrowdControlCosmeticData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "CCMontageMap")
	TMap<ENetherCrownCrowdControlType, TSoftObjectPtr<UAnimMontage>> CrowdControlMontageMap{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownCrowdControlCosmeticDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownCrowdControlCosmeticData& GetCrowdControlCosmeticData() const { return CrowdControlCosmeticData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "CrowdControlCosmeticData")
	FNetherCrownCrowdControlCosmeticData CrowdControlCosmeticData{};
};
