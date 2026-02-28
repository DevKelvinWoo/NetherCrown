// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownUtilManager.h"

#include "NetherCrown/NetherCrown.h"
#include "Kismet/GameplayStatics.h"
#include "NetherCrown/Data/NetherCrownEffectData.h"
#include "NetherCrown/Data/NetherCrownSoundData.h"
#include "NetherCrown/Data/NetherCrownWeaponData.h"
#include "NetherCrown/Settings/NetherCrownDefaultSettings.h"
#include "Sound/SoundCue.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

bool FNetherCrownUtilManager::bCacheInitialized{ false };
TMap<FGameplayTag, FNetherCrownSoundData*> FNetherCrownUtilManager::CachedSoundDataByTag{};
TMap<FGameplayTag, FNetherCrownWeaponDataTableRow*> FNetherCrownUtilManager::CachedWeaponDataByTag{};
TMap<FGameplayTag, FNetherCrownEffectData*> FNetherCrownUtilManager::CachedEffectDataByTag{};

TObjectPtr<UDataTable> FNetherCrownUtilManager::CachedSoundDT{};
TObjectPtr<UDataTable> FNetherCrownUtilManager::CachedWeaponDT{};
TObjectPtr<UDataTable> FNetherCrownUtilManager::CachedEffectDT{};

void FNetherCrownUtilManager::EnsureCacheBuilt()
{
	if (bCacheInitialized)
	{
		return;
	}

	const UNetherCrownDefaultSettings* DefaultSettings{ GetDefault<UNetherCrownDefaultSettings>() };
	check(DefaultSettings);

	const UDataTable* SoundDT{ DefaultSettings->CharacterSoundDT.LoadSynchronous() };
	if (IsValid(SoundDT))
	{
		TArray<FNetherCrownSoundData*> OutRows{};
		SoundDT->GetAllRows<FNetherCrownSoundData>(TEXT("SoundTag"), OutRows);
		for (FNetherCrownSoundData* Row : OutRows)
		{
			if (Row)
			{
				CachedSoundDataByTag.Add(Row->GetSoundTag(), Row);
			}
		}
	}

	const UDataTable* WeaponDT{ DefaultSettings->WeaponDT.LoadSynchronous() };
	if (IsValid(WeaponDT))
	{
		TArray<FNetherCrownWeaponDataTableRow*> OutRows{};
		WeaponDT->GetAllRows<FNetherCrownWeaponDataTableRow>(TEXT("WeaponTag"), OutRows);
		for (FNetherCrownWeaponDataTableRow* Row : OutRows)
		{
			if (Row)
			{
				CachedWeaponDataByTag.Add(Row->GetWeaponTag(), Row);
			}
		}
	}

	const UDataTable* EffectDT{ DefaultSettings->EffectDT.LoadSynchronous() };
	if (IsValid(EffectDT))
	{
		TArray<FNetherCrownEffectData*> OutRows{};
		EffectDT->GetAllRows<FNetherCrownEffectData>(TEXT("EffectTag"), OutRows);
		for (FNetherCrownEffectData* Row : OutRows)
		{
			if (Row)
			{
				CachedEffectDataByTag.Add(Row->GetEffectTag(), Row);
			}
		}
	}

	CachedSoundDT = DefaultSettings->CharacterSoundDT.LoadSynchronous();
	CachedWeaponDT = DefaultSettings->WeaponDT.LoadSynchronous();
	CachedEffectDT = DefaultSettings->EffectDT.LoadSynchronous();

	bCacheInitialized = true;
}

USoundCue* FNetherCrownUtilManager::GetSoundCueByGameplayTag(const FGameplayTag& SoundTag)
{
	if (!SoundTag.IsValid())
	{
		return nullptr;
	}

	EnsureCacheBuilt();

	if (!ensureAlways(IsValid(CachedSoundDT)))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("There is No SoundDataTable in %hs"), __FUNCTION__);

		return nullptr;
	}

	FNetherCrownSoundData** FoundSoundData{ CachedSoundDataByTag.Find(SoundTag) };
	if (!FoundSoundData || !(*FoundSoundData))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("There is No Found SoundDataTable in %hs"), __FUNCTION__);

		return nullptr;
	}
	return (*FoundSoundData)->GetSoundCue().LoadSynchronous();
}

void FNetherCrownUtilManager::PlaySound2DByGameplayTag(const UObject* WorldContextObject, const FGameplayTag& SoundTag)
{
	if (!ensureAlways(WorldContextObject))
	{
		return;
	}

	const UWorld* World{ WorldContextObject->GetWorld() };
	check(World);

	if (World->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	USoundCue* SoundCue{ GetSoundCueByGameplayTag(SoundTag) };
	if (!ensureAlways(SoundCue))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("There is No SoundCue in %hs"), __FUNCTION__);
		return;
	}

	UGameplayStatics::PlaySound2D(WorldContextObject, SoundCue);
}

UNetherCrownWeaponData* FNetherCrownUtilManager::GetWeaponDataByGameplayTag(const FGameplayTag& WeaponTag)
{
	EnsureCacheBuilt();

	if (!ensureAlways(IsValid(CachedWeaponDT)))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("There is No WeaponDataTable in %hs"), __FUNCTION__);

		return nullptr;
	}

	FNetherCrownWeaponDataTableRow** FoundWeaponData{ CachedWeaponDataByTag.Find(WeaponTag) };
	if (!FoundWeaponData || !(*FoundWeaponData))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("There is No Found WeaponDataTable in %hs"), __FUNCTION__);

		return nullptr;
	}

	return (*FoundWeaponData)->GetWeaponData().LoadSynchronous();
}

UNiagaraSystem* FNetherCrownUtilManager::GetNiagaraSystemByGameplayTag(const FGameplayTag& EffectTag)
{
	EnsureCacheBuilt();

	if (!ensureAlways(IsValid(CachedEffectDT)))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("There is No EffectDataTable in %hs"), __FUNCTION__);

		return nullptr;
	}

	FNetherCrownEffectData** FoundEffectData{ CachedEffectDataByTag.Find(EffectTag) };
	if (!FoundEffectData || !(*FoundEffectData))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("Effect Data Not Found for Tag: %s"), *EffectTag.ToString());
		return nullptr;
	}

	return (*FoundEffectData)->GetEffectNiagaraSystem().LoadSynchronous();
}

void FNetherCrownUtilManager::SpawnNiagaraSystemByGameplayTag(const UObject* WorldContextObject, const FGameplayTag& EffectTag, const FTransform& SpawnTransform)
{
	if (!ensureAlways(WorldContextObject))
	{
		return;
	}

	const UWorld* World{ WorldContextObject->GetWorld() };
	check(World);

	if (World->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	UNiagaraSystem* NiagaraSystem{ GetNiagaraSystemByGameplayTag(EffectTag) };
	if (!ensureAlways(IsValid(NiagaraSystem)))
	{
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContextObject, NiagaraSystem, SpawnTransform.GetLocation(), SpawnTransform.GetRotation().Rotator(), SpawnTransform.GetScale3D());
}

UNiagaraComponent* FNetherCrownUtilManager::AttachNiagaraSystemByGameplayTag(const UObject* WorldContextObject, const FGameplayTag& EffectTag, USceneComponent* AttachComponent, const FName AttachSocketName)
{
	if (!ensureAlways(WorldContextObject))
	{
		return nullptr;
	}

	UNiagaraSystem* NiagaraSystem{ GetNiagaraSystemByGameplayTag(EffectTag) };
	if (!ensureAlways(IsValid(NiagaraSystem)))
	{
		return nullptr;
	}

	return UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem, AttachComponent, AttachSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
}
