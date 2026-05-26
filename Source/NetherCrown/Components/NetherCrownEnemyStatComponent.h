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

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnEnemyHPModified, const float);

public:
	UNetherCrownEnemyStatComponent();

	const FNetherCrownEnemyStat& GetEnemyStatData() const { return EnemyStatData; }

	FOnEnemyHPModified& GetOnEnemyHPModified() { return OnEnemyHPModified; }

	void ModifyEnemyHp(float HpDelta);

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void CacheOwnerEnemy();

	void LoadEnemyStatData();
	float GetEnemyHPRatio() const;

	UFUNCTION()
	void OnRep_EnemyStatData(const FNetherCrownEnemyStat& OldEnemyStatData);

	UPROPERTY(Transient, ReplicatedUsing = OnRep_EnemyStatData)
	FNetherCrownEnemyStat EnemyStatData{};

	UPROPERTY(EditDefaultsOnly, Category = "StatDataAsset")
	TSoftObjectPtr<UNetherCrownEnemyStatData> EnemyStatDataAssetSoft{};

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownEnemy> CachedOwnerEnemy{};

	FOnEnemyHPModified OnEnemyHPModified{};
};
