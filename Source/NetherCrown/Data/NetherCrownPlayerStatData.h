// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NetherCrownPlayerStatData.generated.h"

USTRUCT()
struct FNetherCrownPlayerStat
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int32 CharacterHP{ 100 };

	UPROPERTY(EditAnywhere)
	float CharacterMaxHP{ 100.f };

	UPROPERTY(EditAnywhere)
	float CharacterMP{ 100.f };

	UPROPERTY(EditAnywhere)
	float CharacterMaxMP{ 100.f };

	UPROPERTY(EditAnywhere)
	int32 CharacterEXP{ 0 };

	UPROPERTY(EditAnywhere)
	int32 CharacterLevel{ 1 };

	UPROPERTY(EditAnywhere)
	int32 CharacterMaxLevel{ 10 };

	UPROPERTY(EditAnywhere)
	int32 CharacterGold{ 0 };

	UPROPERTY(EditAnywhere)
	int32 CharacterMaxGold{ 1000 };

	UPROPERTY(EditAnywhere)
	int32 PhysicalArmor{ 10 };

	UPROPERTY(EditAnywhere)
	int32 MagicArmor{ 10 };

	UPROPERTY(EditAnywhere)
	int32 AttackDamage{ 10 };

	UPROPERTY(EditAnywhere)
	int32 MagicPower{ 10 };

	UPROPERTY(EditAnywhere)
	int32 ShieldValue{ 0 };
};

UCLASS()
class NETHERCROWN_API UNetherCrownPlayerStatData : public UDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownPlayerStat& GetPlayerStatData() const { return PlayerStatData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "StatData")
	FNetherCrownPlayerStat PlayerStatData{};
};
