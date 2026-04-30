// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NetherCrownEnemyRangedBasicAttackDataAsset.generated.h"

class UNetherCrownEnemyProjectileDataAsset;
class ANetherCrownEnemyMagicProjectile;

USTRUCT()
struct FNetherCrownEnemyRangedBasicAttackComboData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "ComboData")
	FName ComboMontageSectionName{};

	UPROPERTY(EditDefaultsOnly, Category = "ComboData")
	float ComboEndTime{ 0.f };

	UPROPERTY(EditDefaultsOnly, Category = "EnemyRangedBasicAttackData")
	FName FireSocketName{};
};

USTRUCT()
struct FNetherCrownEnemyRangedBasicAttackCosmeticData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "AnimMontage")
	TSoftObjectPtr<UAnimMontage> FireAnimMontageSoft{};
};

USTRUCT()
struct FNetherCrownEnemyRangedBasicAttackData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "EnemyRangedBasicAttackData")
	TSoftObjectPtr<UNetherCrownEnemyProjectileDataAsset> EnemyProjectileDataAssetSoft{};

	UPROPERTY(EditDefaultsOnly, Category = "EnemyRangedBasicAttackData")
	FNetherCrownEnemyRangedBasicAttackCosmeticData EnemyRangedBasicAttackCosmeticData{};

	UPROPERTY(EditDefaultsOnly, Category = "EnemyRangedBasicAttackData")
	TMap<int32, FNetherCrownEnemyRangedBasicAttackComboData> EnemyRangedBasicAttackComboDataMap{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownEnemyRangedBasicAttackDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownEnemyRangedBasicAttackData& GetEnemyRangedBasicAttackData() const { return EnemyRangedBasicAttackData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "EnemyRangedBasicAttackData")
	FNetherCrownEnemyRangedBasicAttackData EnemyRangedBasicAttackData{};
};
