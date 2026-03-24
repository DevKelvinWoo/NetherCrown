// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NetherCrown/Components/NetherCrownPPTypes.h"
#include "NetherCrownPostProcessCosmeticData.generated.h"

class UCurveFloat;

USTRUCT()
struct FNetherCrownPostProcessCosmeticData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TMap<ENetherCrownPPType, FPostProcessSettings> PostProcessSettingsMap{};

	UPROPERTY(EditDefaultsOnly, Category = "Curve")
	TSoftObjectPtr<UCurveFloat> PostProcessStartCurveFloatSoft{};

	UPROPERTY(EditDefaultsOnly, Category = "Curve")
	TSoftObjectPtr<UCurveFloat> PostProcessEndCurveFloatSoft{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownPostProcessCosmeticDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownPostProcessCosmeticData& GetPostProcessCosmeticData() const { return PostProcessCosmeticData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "PostProcessCosmeticData")
	FNetherCrownPostProcessCosmeticData PostProcessCosmeticData{};
};
