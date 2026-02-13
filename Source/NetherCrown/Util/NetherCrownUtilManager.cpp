// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownUtilManager.h"

#include "Kismet/GameplayStatics.h"
#include "NetherCrown/Data/NetherCrownEffectData.h"
#include "NetherCrown/Data/NetherCrownSoundData.h"
#include "NetherCrown/Data/NetherCrownWeaponData.h"
#include "NetherCrown/Settings/NetherCrownDefaultSettings.h"
#include "Sound/SoundCue.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

USoundCue* FNetherCrownUtilManager::GetSoundCueByGameplayTag(const FGameplayTag& SoundTag)
{
	if (!SoundTag.IsValid())
	{
		return nullptr;
	}

	const UNetherCrownDefaultSettings* DefaultSettings{ GetDefault<UNetherCrownDefaultSettings>() };
	check(DefaultSettings);

	const UDataTable* SoundDT{ DefaultSettings->CharacterSoundDT.LoadSynchronous() };
	if (!ensureAlways(IsValid(SoundDT)))
	{
		UE_LOG(LogTemp, Warning, TEXT("There is No SoundDataTable in %hs"), __FUNCTION__);

		return nullptr;
	}

	TArray<FNetherCrownSoundData*> OutRows{};
	SoundDT->GetAllRows<FNetherCrownSoundData>(TEXT("SoundTag"), OutRows);

	FNetherCrownSoundData** FoundSoundDataRow = OutRows.FindByPredicate([&SoundTag](FNetherCrownSoundData* Row)
	{
		return Row->SoundTag == SoundTag;
	});

	if (!FoundSoundDataRow)
	{
		UE_LOG(LogTemp, Warning, TEXT("There is No Found SoundDataTable in %hs"), __FUNCTION__);

		return nullptr;
	}

	FNetherCrownSoundData* FoundSoundData = *FoundSoundDataRow;
	if (!FoundSoundData)
	{
		return nullptr;
	}
	return FoundSoundData->SoundCue.LoadSynchronous();
}

void FNetherCrownUtilManager::PlaySound2DByGameplayTag(const UObject* WorldContextObject, const FGameplayTag& SoundTag)
{
	check(WorldContextObject);
	UWorld* World = WorldContextObject->GetWorld();
	if (World && World->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	USoundCue* SoundCue{ GetSoundCueByGameplayTag(SoundTag) };
	if (!ensureAlways(SoundCue))
	{
		UE_LOG(LogTemp, Warning, TEXT("There is No SoundCue in %hs"), __FUNCTION__);
		return;
	}

	UGameplayStatics::PlaySound2D(WorldContextObject, SoundCue);
}

UNetherCrownWeaponData* FNetherCrownUtilManager::GetWeaponDataByGameplayTag(const FGameplayTag& WeaponTag)
{
	const UNetherCrownDefaultSettings* DefaultSettings{ GetDefault<UNetherCrownDefaultSettings>() };
	check(DefaultSettings);

	const UDataTable* WeaponDT{ DefaultSettings->WeaponDT.LoadSynchronous() };
	if (!ensureAlways(IsValid(WeaponDT)))
	{
		UE_LOG(LogTemp, Warning, TEXT("There is No WeaponDataTable in %hs"), __FUNCTION__);

		return nullptr;
	}

	TArray<FNetherCrownWeaponDataTableRow*> OutRows{};
	WeaponDT->GetAllRows<FNetherCrownWeaponDataTableRow>(TEXT("WeaponTag"), OutRows);
	if (OutRows.IsEmpty())
	{
		return nullptr;
	}

	FNetherCrownWeaponDataTableRow** FoundWeaponDataTableRow{ OutRows.FindByPredicate([&WeaponTag](FNetherCrownWeaponDataTableRow* WeaponDataTableRow)
	{
		return WeaponTag == WeaponDataTableRow->GetWeaponTag();
	}) };

	if (!FoundWeaponDataTableRow)
	{
		UE_LOG(LogTemp, Warning, TEXT("There is No Found WeaponDataTable in %hs"), __FUNCTION__);

		return nullptr;
	}

	FNetherCrownWeaponDataTableRow* FoundWeaponData = *FoundWeaponDataTableRow;
	if (!FoundWeaponData)
	{
		return nullptr;
	}

	return FoundWeaponData->GetWeaponData().LoadSynchronous();
}

UNiagaraSystem* FNetherCrownUtilManager::GetNiagaraSystemByGameplayTag(const FGameplayTag& EffectTag)
{
	const UNetherCrownDefaultSettings* DefaultSettings{ GetDefault<UNetherCrownDefaultSettings>() };
	check(DefaultSettings);

	const UDataTable* EffectDT{ DefaultSettings->EffectDT.LoadSynchronous() };
	if (!ensureAlways(IsValid(EffectDT)))
	{
		UE_LOG(LogTemp, Warning, TEXT("There is No EffectDataTable in %hs"), __FUNCTION__);

		return nullptr;
	}

	TArray<FNetherCrownEffectData*> OutRows{};
	EffectDT->GetAllRows<FNetherCrownEffectData>(TEXT("EffectTag"), OutRows);

	FNetherCrownEffectData** FoundEffectDataRow{ OutRows.FindByPredicate([&EffectTag](FNetherCrownEffectData* EffectData)
	{
		return EffectData->EffectTag == EffectTag;
	}) };

	FNetherCrownEffectData* FoundEffectData = *FoundEffectDataRow;
	if (!FoundEffectData)
	{
		return nullptr;
	}

	return FoundEffectData->EffectNiagaraSystem.LoadSynchronous();
}

void FNetherCrownUtilManager::SpawnNiagaraSystemByGameplayTag(const UObject* WorldContextObject, const FGameplayTag& EffectTag, const FTransform& SpawnTransform)
{
	if (!ensureAlways(WorldContextObject))
	{
		return;
	}

	UNiagaraSystem* NiagaraSystem{ GetNiagaraSystemByGameplayTag(EffectTag) };
	if (!ensureAlways(IsValid(NiagaraSystem)))
	{
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContextObject, NiagaraSystem,SpawnTransform.GetLocation(), SpawnTransform.GetRotation().Rotator(), SpawnTransform.GetScale3D());
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
