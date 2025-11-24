// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownUtilManager.h"

#include "NetherCrown/Data/NetherCrownSoundData.h"
#include "NetherCrown/Settings/NetherCrownDefaultSettings.h"
#include "Sound/SoundCue.h"

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

	FNetherCrownSoundData** FoundSoundData = OutRows.FindByPredicate([&SoundTag](FNetherCrownSoundData* Row)
	{
		return Row->SoundTag == SoundTag;
	});

	FNetherCrownSoundData* FoundedSoundData = *FoundSoundData;
	if (!FoundedSoundData)
	{
		return nullptr;
	}
	return FoundedSoundData->SoundCue.LoadSynchronous();
}
