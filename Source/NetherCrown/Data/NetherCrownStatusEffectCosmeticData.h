// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NetherCrown/Components/NetherCrownCrowdControlTypes.h"
#include "NetherCrownStatusEffectCosmeticData.generated.h"

class UNiagaraSystem;

USTRUCT()
struct FNetherCrownStatusEffectCosmeticData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "StatusCosmeticData")
	TMap<ENetherCrownCrowdControlType, TSoftObjectPtr<UNiagaraSystem>> StatusCosmeticMap{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownStatusEffectCosmeticDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownStatusEffectCosmeticData& GetStatusEffectCosmeticData() const { return StatusEffectCosmeticData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "StatusEffectCosmeticData")
	FNetherCrownStatusEffectCosmeticData StatusEffectCosmeticData{};
};
