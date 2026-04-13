// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownUtilManager.h"

#include "NetherCrown/NetherCrown.h"
#include "Kismet/GameplayStatics.h"
#include "NetherCrown/Data/NetherCrownEffectData.h"
#include "NetherCrown/Data/NetherCrownSkillData.h"
#include "NetherCrown/Data/NetherCrownSoundData.h"
#include "NetherCrown/Data/NetherCrownWeaponData.h"
#include "NetherCrown/Settings/NetherCrownDefaultSettings.h"
#include "Sound/SoundCue.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NetherCrown/UI/NetherCrownUITypes.h"

bool FNetherCrownUtilManager::bCacheInitialized{ false };
TMap<FGameplayTag, FNetherCrownSoundData> FNetherCrownUtilManager::CachedSoundDataByTag{};
TMap<FGameplayTag, FNetherCrownSkillDataTableRow> FNetherCrownUtilManager::CachedSkillDataByTag{};
TMap<FGameplayTag, FNetherCrownWeaponDataTableRow> FNetherCrownUtilManager::CachedWeaponDataByTag{};
TMap<FGameplayTag, FNetherCrownEffectData> FNetherCrownUtilManager::CachedEffectDataByTag{};
TMap<FGameplayTag, FNetherCrownUIScreenDefinition> FNetherCrownUtilManager::CachedScreenDefinitionDataByTag{};
TMap<FGameplayTag, FNetherCrownEnemySkillDataTableRow> FNetherCrownUtilManager::CachedEnemySkillDataByTag{};

void FNetherCrownUtilManager::EnsureCacheBuilt()
{
	if (bCacheInitialized)
	{
		return;
	}

	const UNetherCrownDefaultSettings* DefaultSettings{ GetDefault<UNetherCrownDefaultSettings>() };
	check(DefaultSettings);

	CachedSoundDataByTag.Empty();
	CachedSkillDataByTag.Empty();
	CachedEnemySkillDataByTag.Empty();
	CachedWeaponDataByTag.Empty();
	CachedEffectDataByTag.Empty();
	CachedScreenDefinitionDataByTag.Empty();

	UDataTable* SoundDT{ DefaultSettings->CharacterSoundDT.LoadSynchronous() };
	if (IsValid(SoundDT))
	{
		TArray<FNetherCrownSoundData*> OutRows{};
		SoundDT->GetAllRows<FNetherCrownSoundData>(TEXT("SoundTag"), OutRows);
		for (FNetherCrownSoundData* Row : OutRows)
		{
			if (Row)
			{
				CachedSoundDataByTag.Add(Row->GetSoundTag(), *Row);
			}
		}
	}

	UDataTable* WeaponDT{ DefaultSettings->WeaponDT.LoadSynchronous() };
	if (IsValid(WeaponDT))
	{
		TArray<FNetherCrownWeaponDataTableRow*> OutRows{};
		WeaponDT->GetAllRows<FNetherCrownWeaponDataTableRow>(TEXT("WeaponTag"), OutRows);
		for (FNetherCrownWeaponDataTableRow* Row : OutRows)
		{
			if (Row)
			{
				CachedWeaponDataByTag.Add(Row->GetWeaponTag(), *Row);
			}
		}
	}

	UDataTable* SkillDT{ DefaultSettings->SkillDT.LoadSynchronous() };
	if (IsValid(SkillDT))
	{
		TArray<FNetherCrownSkillDataTableRow*> OutRows{};
		SkillDT->GetAllRows<FNetherCrownSkillDataTableRow>(TEXT("SkillTag"), OutRows);
		for (FNetherCrownSkillDataTableRow* Row : OutRows)
		{
			if (Row)
			{
				CachedSkillDataByTag.Add(Row->GetSkillTag(), *Row);
			}
		}
	}

	UDataTable* EnemySkillDT{ DefaultSettings->EnemySkillDT.LoadSynchronous() };
	if (IsValid(EnemySkillDT))
	{
		TArray<FNetherCrownEnemySkillDataTableRow*> OutRows{};
		EnemySkillDT->GetAllRows<FNetherCrownEnemySkillDataTableRow>(TEXT("EnemySkillTag"), OutRows);
		for (FNetherCrownEnemySkillDataTableRow* Row : OutRows)
		{
			if (Row)
			{
				CachedEnemySkillDataByTag.Add(Row->GetEnemySkillTag(), *Row);
			}
		}
	}

	UDataTable* EffectDT{ DefaultSettings->EffectDT.LoadSynchronous() };
	if (IsValid(EffectDT))
	{
		TArray<FNetherCrownEffectData*> OutRows{};
		EffectDT->GetAllRows<FNetherCrownEffectData>(TEXT("EffectTag"), OutRows);
		for (FNetherCrownEffectData* Row : OutRows)
		{
			if (Row)
			{
				CachedEffectDataByTag.Add(Row->GetEffectTag(), *Row);
			}
		}
	}

	UDataTable* ScreenDefinitionDT{ DefaultSettings->ScreenDefinitionDT.LoadSynchronous() };
	if (IsValid(ScreenDefinitionDT))
	{
		TArray<FNetherCrownUIScreenDefinition*> OutRows{};
		ScreenDefinitionDT->GetAllRows<FNetherCrownUIScreenDefinition>(TEXT("ScreenDefinitionTag"), OutRows);
		for (FNetherCrownUIScreenDefinition* Row : OutRows)
		{
			if (Row)
			{
				CachedScreenDefinitionDataByTag.Add(Row->ScreenTag, *Row);
			}
		}
	}

	bCacheInitialized = true;
}

USoundCue* FNetherCrownUtilManager::GetSoundCueByGameplayTag(const FGameplayTag& SoundTag)
{
	if (!SoundTag.IsValid())
	{
		return nullptr;
	}

	EnsureCacheBuilt();

	if (!ensureAlways(!CachedSoundDataByTag.IsEmpty()))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("There is No SoundData in %hs"), __FUNCTION__);

		return nullptr;
	}

	const FNetherCrownSoundData* FoundSoundData{ CachedSoundDataByTag.Find(SoundTag) };
	if (!FoundSoundData)
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("There is No Found SoundDataTable in %hs"), __FUNCTION__);

		return nullptr;
	}

	return FoundSoundData->GetSoundCue().LoadSynchronous();
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

UNetherCrownSkillDataAsset* FNetherCrownUtilManager::GetSkillDataAssetByGameplayTag(const FGameplayTag& SkillTag)
{
	EnsureCacheBuilt();

	if (!ensureAlways(!CachedSkillDataByTag.IsEmpty()))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("There is No SkillData in %hs"), __FUNCTION__);

		return nullptr;
	}

	const FNetherCrownSkillDataTableRow* FoundSkillData{ CachedSkillDataByTag.Find(SkillTag) };
	if (!FoundSkillData)
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("There is No Found SkillDataTable in %hs"), __FUNCTION__);

		return nullptr;
	}

	return FoundSkillData->GetSkillDataAsset().LoadSynchronous();
}

UNetherCrownEnemySkillDataAsset* FNetherCrownUtilManager::GetEnemySkillDataAssetByGameplayTag(const FGameplayTag& SkillTag)
{
	EnsureCacheBuilt();

	if (!ensureAlways(!CachedEnemySkillDataByTag.IsEmpty()))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("There is No EnemySkillData in %hs"), __FUNCTION__);

		return nullptr;
	}

	const FNetherCrownEnemySkillDataTableRow* FoundSkillData{ CachedEnemySkillDataByTag.Find(SkillTag) };
	if (!FoundSkillData)
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("There is No Found SkillDataTable in %hs"), __FUNCTION__);

		return nullptr;
	}

	return FoundSkillData->GetEnemySkillDataAsset().LoadSynchronous();
}

UNetherCrownWeaponData* FNetherCrownUtilManager::GetWeaponDataByGameplayTag(const FGameplayTag& WeaponTag)
{
	EnsureCacheBuilt();

	if (!ensureAlways(!CachedWeaponDataByTag.IsEmpty()))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("There is No WeaponData in %hs"), __FUNCTION__);

		return nullptr;
	}

	const FNetherCrownWeaponDataTableRow* FoundWeaponData{ CachedWeaponDataByTag.Find(WeaponTag) };
	if (!FoundWeaponData)
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("There is No Found WeaponDataTable in %hs"), __FUNCTION__);

		return nullptr;
	}

	return FoundWeaponData->GetWeaponData().LoadSynchronous();
}

UNiagaraSystem* FNetherCrownUtilManager::GetNiagaraSystemByGameplayTag(const FGameplayTag& EffectTag)
{
	EnsureCacheBuilt();

	if (!ensureAlways(!CachedEffectDataByTag.IsEmpty()))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("There is No EffectData in %hs"), __FUNCTION__);

		return nullptr;
	}

	const FNetherCrownEffectData* FoundEffectData{ CachedEffectDataByTag.Find(EffectTag) };
	if (!FoundEffectData)
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("Effect Data Not Found for Tag: %s"), *EffectTag.ToString());
		return nullptr;
	}

	return FoundEffectData->GetEffectNiagaraSystem().LoadSynchronous();
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

const TArray<FNetherCrownUIScreenDefinition> FNetherCrownUtilManager::GetUIScreenDefinitionData()
{
	EnsureCacheBuilt();

	TArray<FNetherCrownUIScreenDefinition> ScreenDefinitionArray{};
	for (const TPair<FGameplayTag, FNetherCrownUIScreenDefinition>& ScreenDefinitionPair : CachedScreenDefinitionDataByTag)
	{
		ScreenDefinitionArray.Add(ScreenDefinitionPair.Value);
	}

	return ScreenDefinitionArray;
}
