// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "NetherCrown/Data/NetherCrownEffectData.h"
#include "NetherCrown/Data/NetherCrownEnemySkillData.h"
#include "NetherCrown/Data/NetherCrownLevelSequenceData.h"
#include "NetherCrown/Data/NetherCrownQuestData.h"
#include "NetherCrown/Data/NetherCrownSkillData.h"
#include "NetherCrown/Data/NetherCrownSoundData.h"
#include "NetherCrown/Data/NetherCrownWeaponData.h"
#include "NetherCrown/UI/NetherCrownUITypes.h"

class USoundCue;
class UNiagaraSystem;
class UNiagaraComponent;
class ULevelSequence;

class UNetherCrownSkillDataAsset;
class UNetherCrownWeaponData;
class UNetherCrownQuestData;

class FNetherCrownUtilManager
{
public:
	//Sound
	static USoundCue* GetSoundCueByGameplayTag(const FGameplayTag& SoundTag);
	static void PlaySound2DByGameplayTag(const UObject* WorldContextObject, const FGameplayTag& SoundTag);

	//SkillData
	static UNetherCrownSkillDataAsset* GetSkillDataAssetByGameplayTag(const FGameplayTag& SkillTag);
	static UNetherCrownEnemySkillDataAsset* GetEnemySkillDataAssetByGameplayTag(const FGameplayTag& SkillTag);

	//WeaponData
	static UNetherCrownWeaponData* GetWeaponDataByGameplayTag(const FGameplayTag& WeaponTag);

	//Effect
	static UNiagaraSystem* GetNiagaraSystemByGameplayTag(const FGameplayTag& EffectTag);
	static void SpawnNiagaraSystemByGameplayTag(const UObject* WorldContextObject, const FGameplayTag& EffectTag, const FTransform& SpawnTransform);
	static UNiagaraComponent* AttachNiagaraSystemByGameplayTag(const UObject* WorldContextObject, const FGameplayTag& EffectTag, USceneComponent* AttachComponent, const FName AttachSocketName);

	//UI
	static const TArray<FNetherCrownUIScreenDefinition> GetUIScreenDefinitionData();

	//Sequence
	static ULevelSequence* GetLevelSequenceByGameplayTag(const FGameplayTag& SequenceTag);

	//Quest
	static UNetherCrownQuestData* GetQuestDataAssetByGameplayTag(const FGameplayTag& QuestTag);

private:
	static void EnsureCacheBuilt();

	static bool bCacheInitialized;
	static TMap<FGameplayTag, FNetherCrownSoundData> CachedSoundDataByTag;
	static TMap<FGameplayTag, FNetherCrownSkillDataTableRow> CachedSkillDataByTag;
	static TMap<FGameplayTag, FNetherCrownEnemySkillDataTableRow> CachedEnemySkillDataByTag;
	static TMap<FGameplayTag, FNetherCrownWeaponDataTableRow> CachedWeaponDataByTag;
	static TMap<FGameplayTag, FNetherCrownEffectData> CachedEffectDataByTag;
	static TMap<FGameplayTag, FNetherCrownUIScreenDefinition> CachedScreenDefinitionDataByTag;
	static TMap<FGameplayTag, FNetherCrownLevelSequenceData> CachedLevelSequenceByTag;
	static TMap<FGameplayTag, FNetherCrownQuestDataTableRow> CachedQuestDataByTag;

	static TMap<FGameplayTag, TWeakObjectPtr<USoundCue>> CachedSoundCueByTag;
	static TMap<FGameplayTag, TWeakObjectPtr<UNetherCrownSkillDataAsset>> CachedSkillDataAssetByTag;
	static TMap<FGameplayTag, TWeakObjectPtr<UNetherCrownEnemySkillDataAsset>> CachedEnemySkillDataAssetByTag;
	static TMap<FGameplayTag, TWeakObjectPtr<UNetherCrownWeaponData>> CachedWeaponDataAssetByTag;
	static TMap<FGameplayTag, TWeakObjectPtr<UNiagaraSystem>> CachedNiagaraSystemByTag;
	static TMap<FGameplayTag, TWeakObjectPtr<ULevelSequence>> CachedLevelSequenceAssetByTag;
	static TMap<FGameplayTag, TWeakObjectPtr<UNetherCrownQuestData>> CachedQuestDataAssetByTag;
};
