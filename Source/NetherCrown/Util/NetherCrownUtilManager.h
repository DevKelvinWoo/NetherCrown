// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class USoundCue;
class UNiagaraSystem;
class UNiagaraComponent;

class UNetherCrownWeaponData;
struct FNetherCrownEffectData;
struct FNetherCrownSoundData;
struct FNetherCrownWeaponDataTableRow;

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
	static UNiagaraComponent* AttachNiagaraSystemByGameplayTag(const UObject* WorldContextObject, const FGameplayTag& EffectTag, USceneComponent* AttachComponent, const FName AttachSocketName);

private:
	static void EnsureCacheBuilt();

	static bool bCacheInitialized;
	static TMap<FGameplayTag, FNetherCrownSoundData*> CachedSoundDataByTag;
	static TMap<FGameplayTag, FNetherCrownWeaponDataTableRow*> CachedWeaponDataByTag;
	static TMap<FGameplayTag, FNetherCrownEffectData*> CachedEffectDataByTag;

	static TObjectPtr<UDataTable> CachedSoundDT;
	static TObjectPtr<UDataTable> CachedWeaponDT;
	static TObjectPtr<UDataTable> CachedEffectDT;
};
