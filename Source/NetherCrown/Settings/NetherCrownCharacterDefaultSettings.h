// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "NetherCrownCharacterDefaultSettings.generated.h"

class UDataTable;

UCLASS()
class NETHERCROWN_API UNetherCrownCharacterDefaultSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float MinHardLandingHeight{ 200.f };

	UPROPERTY(EditAnywhere)
	float RecoveryResetDelayTime{ 1.f };
};
