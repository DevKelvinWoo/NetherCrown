// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrown/Data/NetherCrownEnemyStatData.h"
#include "NetherCrownEnemyStatComponent.generated.h"

class ANetherCrownEnemy;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownEnemyStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownEnemyStatComponent();

	const FNetherCrownEnemyStat& GetEnemyStatData() const { return EnemyStatData; }

	void ModifyEnemyHp(float HpDelta);

protected:
	virtual void BeginPlay() override;

private:
	void CacheOwnerEnemy();

	void LoadEnemyStatData();

	UPROPERTY(Transient)
	FNetherCrownEnemyStat EnemyStatData{};

	UPROPERTY(EditDefaultsOnly, Category = "StatDataAsset")
	TSoftObjectPtr<UNetherCrownEnemyStatData> EnemyStatDataAssetSoft{};

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownEnemy> CachedOwnerEnemy{};
};
