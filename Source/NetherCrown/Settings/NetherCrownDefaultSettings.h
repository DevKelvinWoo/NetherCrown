// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "NetherCrownDefaultSettings.generated.h"

class UDataTable;

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "NetherCrown Default Settings"))
class NETHERCROWN_API UNetherCrownDefaultSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere)
	TSoftObjectPtr<UDataTable> CharacterSoundDT;

	UPROPERTY(Config, EditAnywhere)
	TSoftObjectPtr<UDataTable> WeaponDT;

	UPROPERTY(Config, EditAnywhere)
	TSoftObjectPtr<UDataTable> EffectDT;
};
