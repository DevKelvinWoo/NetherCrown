// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrownEnemyRangedBasicAttackComponent.generated.h"

class UNetherCrownEnemyRangedBasicAttackDataAsset;
class UNetherCrownEnemyProjectileDataAsset;
class ANetherCrownEnemy;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownEnemyRangedBasicAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownEnemyRangedBasicAttackComponent();

	void RequestEnemyRangedAttack();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void CacheEnemyRangedBasicAttackData();
	void CacheInitData();

	void SpawnRangedBasicAttackProjectile();
	FVector GetProjectileSpawnLocation() const;

	UPROPERTY(Transient)
	TObjectPtr<UNetherCrownEnemyRangedBasicAttackDataAsset> CachedEnemyRangedBasicAttackDataAsset{};

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TSoftObjectPtr<UNetherCrownEnemyRangedBasicAttackDataAsset> EnemyRangedBasicAttackDataAssetSoft{};

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownEnemy> CachedOwnerEnemy{};

	UPROPERTY(Transient)
	TObjectPtr<UNetherCrownEnemyProjectileDataAsset> CachedProjectileData{};
};
