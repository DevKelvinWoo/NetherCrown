// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownLevelTravelManagerSubsystem.h"

#include "NetherCrown/Data/NetherCrownLevelTravelDataAsset.h"
#include "NetherCrown/Data/NetherCrownSkillData.h"
#include "NetherCrown/Settings/NetherCrownDefaultSettings.h"

void UNetherCrownLevelTravelManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CacheLevelTravelDT();
}

const UNetherCrownLevelTravelDataAsset* UNetherCrownLevelTravelManagerSubsystem::GetLevelTravelData(const FGameplayTag& LevelTag) const
{
	if (!ensureAlways(CachedLevelTravelDT))
	{
		return nullptr;
	}

	TArray<FNetherCrownLevelTravelDataTableRow*> LevelTravelDataArray{};
	CachedLevelTravelDT->GetAllRows<FNetherCrownLevelTravelDataTableRow>(TEXT("LevelTravelTag"), LevelTravelDataArray);

	for (FNetherCrownLevelTravelDataTableRow* LevelTravelData : LevelTravelDataArray)
	{
		const UNetherCrownLevelTravelDataAsset* LevelTravelDataAsset{ LevelTravelData->GetLevelTravelDataAsset() };
		if (!IsValid(LevelTravelDataAsset))
		{
			continue;
		}

		const FNetherCrownLevelTravelData& FoundLevelTravelData{ LevelTravelDataAsset->GetLevelTravelData() };
		if (FoundLevelTravelData.LevelTravelTag == LevelTag)
		{
			return LevelTravelDataAsset;
		}
	}

	return nullptr;
}

void UNetherCrownLevelTravelManagerSubsystem::CacheLevelTravelDT()
{
	const UNetherCrownDefaultSettings* DefaultSettings{ GetDefault<UNetherCrownDefaultSettings>() };
	check(DefaultSettings);

	const TSoftObjectPtr<UDataTable> LevelTravelDTSoft{ DefaultSettings->GetLevelTravelDT() };
	if (LevelTravelDTSoft.IsNull())
	{
		return;
	}

	CachedLevelTravelDT = LevelTravelDTSoft.LoadSynchronous();
}
