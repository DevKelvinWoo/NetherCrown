// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NetherCrownNPCData.generated.h"

UCLASS()
class NETHERCROWN_API UNetherCrownNPCDataAsset : public UDataAsset
{
	GENERATED_BODY()
};

USTRUCT()
struct FNetherCrownWeaponSellNPCData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "DialogueData")
	TMap<int32, FText> WeaponSellNPCDialogueData{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownWeaponSellNPCDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownWeaponSellNPCData& GetWeaponSellNPCData() { return WeaponSellNPCData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	FNetherCrownWeaponSellNPCData WeaponSellNPCData{};
};