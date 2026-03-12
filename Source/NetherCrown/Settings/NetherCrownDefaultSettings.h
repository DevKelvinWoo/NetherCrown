// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "NetherCrownDefaultSettings.generated.h"

class UDataTable;
class UCurveFloat;

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "NetherCrown Default Settings"))
class NETHERCROWN_API UNetherCrownDefaultSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "DataTable")
	TSoftObjectPtr<UDataTable> CharacterSoundDT;

	UPROPERTY(Config, EditAnywhere, Category = "DataTable")
	TSoftObjectPtr<UDataTable> WeaponDT;

	UPROPERTY(Config, EditAnywhere, Category = "DataTable")
	TSoftObjectPtr<UDataTable> EffectDT;

	UPROPERTY(Config, EditAnywhere, Category = "MaterialParam")
	FName FrozenTempestTargetMaterialParam{ TEXT("_VfxMix") };

	UPROPERTY(Config, EditAnywhere, Category = "MaterialParam")
	FName SkyFallSlashArmMaterialParam{ TEXT("SkyFallSlashAlpha") };

	UPROPERTY(Config, EditAnywhere, Category = "MaterialAlpha")
	float FrozenTempestTargetMaterialAlpha{ 0.85f };

	UPROPERTY(Config, EditAnywhere, Category = "Curve")
	TSoftObjectPtr<UCurveFloat> TargetOverlayMaterialEndCurveFloatSoft{};
};
