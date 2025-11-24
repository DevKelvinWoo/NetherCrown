// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class USoundCue;

class FNetherCrownUtilManager
{
public:
	static USoundCue* GetSoundCueByGameplayTag(const FGameplayTag& SoundTag);
};
