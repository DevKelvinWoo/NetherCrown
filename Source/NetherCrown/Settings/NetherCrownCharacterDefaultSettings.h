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
	float GetMinHardLandingHeight() const { return MinHardLandingHeight; }
	float GetRecoveryResetDelayTime() const { return RecoveryResetDelayTime; }
	const FName& GetEquipWeaponSocketName() const { return EquipWeaponSocketName; }
	const FName& GetStowWeaponSocketLName() const { return StowWeaponSocketLName; }
	const FName& GetStowWeaponSocketRName() const { return StowWeaponSocketRName; }
	const FName& GetWeaponTraceSocketName() const { return WeaponTraceSocketName; }

private:
	UPROPERTY(Config, EditAnywhere)
	float MinHardLandingHeight{ 200.f };

	UPROPERTY(Config, EditAnywhere)
	float RecoveryResetDelayTime{ 1.f };

	UPROPERTY(Config, EditAnywhere)
	FName EquipWeaponSocketName{ TEXT("WeaponSocket") };

	UPROPERTY(Config, EditAnywhere)
	FName StowWeaponSocketLName{ TEXT("StowWeaponSocketL") };

	UPROPERTY(Config, EditAnywhere)
	FName StowWeaponSocketRName{ TEXT("StowWeaponSocketR") };

	UPROPERTY(Config, EditAnywhere)
	FName WeaponTraceSocketName{ TEXT("TraceEnd") };
};
