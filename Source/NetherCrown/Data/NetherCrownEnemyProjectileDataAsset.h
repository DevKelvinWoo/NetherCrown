// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NetherCrownEnemyProjectileDataAsset.generated.h"

class ANetherCrownEnemyMagicProjectile;

USTRUCT()
struct FNetherCrownEnemyProjectileData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "EnemyRangedBasicAttackData")
	TSubclassOf<ANetherCrownEnemyMagicProjectile> EnemyMagicProjectileClass{};

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileData")
	float ProjectileSpeed{ 1800.f };
};

UCLASS()
class NETHERCROWN_API UNetherCrownEnemyProjectileDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownEnemyProjectileData& GetEnemyProjectileData() const { return EnemyProjectileData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "ProjectileData")
	FNetherCrownEnemyProjectileData EnemyProjectileData{};
};
