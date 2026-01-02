// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class USoundCue;
class UNiagaraSystem;

class UNetherCrownWeaponData;

class FNetherCrownUtilManager
{
public:
	//Sound
	static USoundCue* GetSoundCueByGameplayTag(const FGameplayTag& SoundTag);
	static void PlaySound2DByGameplayTag(const UObject* WorldContextObject, const FGameplayTag& SoundTag);

	//WeaponData
	static UNetherCrownWeaponData* GetWeaponDataByGameplayTag(const FGameplayTag& WeaponTag);

	//Effect
	static UNiagaraSystem* GetNiagaraSystemByGameplayTag(const FGameplayTag& EffectTag);
	static void SpawnNiagaraSystemByGameplayTag(const UObject* WorldContextObject, const FGameplayTag& EffectTag, const FTransform& SpawnTransform);
};
