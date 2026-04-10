// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "NetherCrownEnemyBTCosmeticData.generated.h"

class UAnimMontage;

USTRUCT()
struct FNetherCrownEnemyBTFoundCosmeticTagData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag FoundGruntSoundTag{};
};

USTRUCT()
struct FNetherCrownEnemyBTFoundCosmeticData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FNetherCrownEnemyBTFoundCosmeticTagData FoundCosmeticTagData{};

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> FoundAnimMontageSoft{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownEnemyBTCosmeticData : public UDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownEnemyBTFoundCosmeticData& GetEnemyFoundCosmeticData() { return EnemyFoundCosmeticData; }

private:
	UPROPERTY(EditDefaultsOnly)
	FNetherCrownEnemyBTFoundCosmeticData EnemyFoundCosmeticData{};
};
