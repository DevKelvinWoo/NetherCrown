// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NetherCrownCrowdControlCosmeticData.generated.h"

class UAnimMontage;
class UCurveFloat;

USTRUCT()
struct FNetherCrownFrozenCosmeticData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Frozen")
	TSoftObjectPtr<UCurveFloat> OverlayEndCurveFloatSoft{};

	UPROPERTY(EditDefaultsOnly, Category = "Frozen")
	FName OverlayMaterialParam{ TEXT("_VfxMix") };

	UPROPERTY(EditDefaultsOnly, Category = "Frozen")
	float OverlayAlpha{ 0.85f };
};

USTRUCT()
struct FNetherCrownCrowdControlCosmeticData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Frozen")
	FNetherCrownFrozenCosmeticData FrozenCosmeticData{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownCrowdControlCosmeticDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownCrowdControlCosmeticData& GetCrowdControlCosmeticData() const { return CrowdControlCosmeticData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "CrowdControlCosmeticData")
	FNetherCrownCrowdControlCosmeticData CrowdControlCosmeticData{};
};
