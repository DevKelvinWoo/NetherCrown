// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "UNetherCrownEnemyDamageAndDeathCosmeticData.generated.h"

class UAnimMontage;

USTRUCT()
struct FNetherCrownEnemyDamageSoundTagData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag EnemyHurtGruntSoundTag{};

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag EnemyHurtImpactSoundTag{};
};

USTRUCT()
struct FNetherCrownEnemyDeathCosmeticData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag EnemyDeathSoundTag{};

	UPROPERTY(EditDefaultsOnly, Category = "Timing")
	float DestroyTimeOffset{};
};

USTRUCT()
struct FUNetherCrownEnemyDamageCosmeticData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "AnimMontage")
	TSoftObjectPtr<UAnimMontage> TakeDamageAnimMontageSoft{};

	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FNetherCrownEnemyDamageSoundTagData DamageSoundTagData{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownEnemyDamageAndDeathCosmeticDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const FUNetherCrownEnemyDamageCosmeticData& GetEnemyDamageCosmeticData() const { return EnemyDamageCosmeticData; }
	const FNetherCrownEnemyDeathCosmeticData& GetEnemyDeathCosmeticData() const { return EnemyDeathCosmeticData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "EnemyDamageCosmeticData")
	FUNetherCrownEnemyDamageCosmeticData EnemyDamageCosmeticData{};

	UPROPERTY(EditDefaultsOnly, Category = "EnemyDeathCosmeticData")
	FNetherCrownEnemyDeathCosmeticData EnemyDeathCosmeticData{};
};
