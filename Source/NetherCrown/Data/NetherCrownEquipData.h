// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "NetherCrownEquipData.generated.h"

class UAnimMontage;

USTRUCT()
struct FNetherCrownEquipTagData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag EquipSoundTag{};
};

USTRUCT()
struct FNetherCrownEquipData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "AnimMontage")
	TSoftObjectPtr<UAnimMontage> EquipAnimMontageSoft{};

	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FNetherCrownEquipTagData EquipTagData{};

	UPROPERTY(EditDefaultsOnly, Category = "EquipTimerData")
	float EquipStartTimeOffset{ -1.f };

	UPROPERTY(EditDefaultsOnly, Category = "EquipTimerData")
	float EquipEndTimeOffset{ -1.f };
};

UCLASS()
class NETHERCROWN_API UNetherCrownEquipDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownEquipData& GetEquipData() const { return EquipData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "EquipData")
	FNetherCrownEquipData EquipData{};
};
