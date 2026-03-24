// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "NetherCrownEnemyDamageCosmeticData.generated.h"

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
struct FNetherCrownEnemyDamageCosmeticData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "AnimMontage")
	TSoftObjectPtr<UAnimMontage> TakeDamageAnimMontageSoft{};

	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FNetherCrownEnemyDamageSoundTagData DamageSoundTagData{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownEnemyDamageCosmeticDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownEnemyDamageCosmeticData& GetEnemyDamageCosmeticData() const { return EnemyDamageCosmeticData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "EnemyDamageCosmeticData")
	FNetherCrownEnemyDamageCosmeticData EnemyDamageCosmeticData{};
};
