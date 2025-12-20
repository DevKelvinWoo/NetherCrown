// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "NetherCrownCharacterDefaultSettings.generated.h"

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "NetherCrown Character Settings"))
class NETHERCROWN_API UNetherCrownCharacterDefaultSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere)
	float MinHardLandingHeight{ 200.f };

	UPROPERTY(Config, EditAnywhere)
	float RecoveryResetDelayTime{ 1.f };

	UPROPERTY(Config, EditAnywhere)
	FName EquipWeaponSocketName{ TEXT("WeaponSocket") };

	UPROPERTY(Config, EditAnywhere)
	FName StowWeaponSocketLName{ TEXT("StowWeaponSocketL") };

	UPROPERTY(Config, EditAnywhere)
	FName WeaponHandleSocketRName{ TEXT("StowWeaponSocketR") };

	UPROPERTY(Config, EditAnywhere)
	FName WeaponTraceSocketName{ TEXT("TraceEnd") };
};
