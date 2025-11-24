// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "NetherCrownDefaultSettings.generated.h"

class UDataTable;

UCLASS()
class NETHERCROWN_API UNetherCrownDefaultSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UDataTable> CharacterSoundDT;
};
