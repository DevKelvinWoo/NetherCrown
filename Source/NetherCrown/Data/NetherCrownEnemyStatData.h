// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NetherCrownEnemyStatData.generated.h"

USTRUCT()
struct FNetherCrownEnemyStat
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int32 EnemyHP{ 100 };

	UPROPERTY(EditAnywhere)
	int32 AttackDamage{ 10 };

	UPROPERTY(EditAnywhere)
	int32 MagicPower{ 10 };

	UPROPERTY(EditAnywhere)
	int32 PhysicalArmor{ 10 };

	UPROPERTY(EditAnywhere)
	int32 MagicArmor{ 10 };

	UPROPERTY(EditAnywhere)
	int32 AttackRange{ 200 };

	UPROPERTY(EditAnywhere)
	float DefaultSpeed{ 300.f };

	UPROPERTY(EditAnywhere)
	float RunChaseSpeed{ 600.f };
};

UCLASS()
class NETHERCROWN_API UNetherCrownEnemyStatData : public UDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownEnemyStat& GetEnemyStatData() const { return EnemyStatData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "StatData")
	FNetherCrownEnemyStat EnemyStatData{};
};
