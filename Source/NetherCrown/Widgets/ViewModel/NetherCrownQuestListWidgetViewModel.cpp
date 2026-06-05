// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownQuestListWidgetViewModel.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownQuestComponent.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

void UNetherCrownQuestListWidgetViewModel::InitViewModel(ANetherCrownCharacter* InModelCharacter)
{
	ModelCharacterWeak = MakeWeakObjectPtr(InModelCharacter);
}

const TArray<FNetherCrownQuestStateEntry>& UNetherCrownQuestListWidgetViewModel::GetCurrentQuestStateEntries() const
{
	const ANetherCrownCharacter* ModelCharacter{ ModelCharacterWeak.Get() };
	if (!IsValid(ModelCharacter))
	{
		static const TArray<FNetherCrownQuestStateEntry> EmptyQuestStateEntries{};
		return EmptyQuestStateEntries;
	}

	const UNetherCrownQuestComponent* QuestComponent{ ModelCharacter->GetQuestComponent() };
	if (!ensureAlways(IsValid(QuestComponent)))
	{
		static const TArray<FNetherCrownQuestStateEntry> EmptyQuestStateEntries{};
		return EmptyQuestStateEntries;
	}

	const TArray<FNetherCrownQuestStateEntry>& QuestStateEntries{ QuestComponent->GetQuestStateEntries() };
	return QuestStateEntries;
}

TArray<FText> UNetherCrownQuestListWidgetViewModel::GetCurrentQuestEntryDescriptions() const
{
	const TArray<FNetherCrownQuestStateEntry>& QuestStateEntries{ GetCurrentQuestStateEntries() };
	if (QuestStateEntries.IsEmpty())
	{
		return {};
	}

	TArray<FText> QuestEntryDescriptions{};
	for (const FNetherCrownQuestStateEntry& QuestStateEntry : QuestStateEntries)
	{
		const FGameplayTag& QuestTag{ QuestStateEntry.QuestTag };
		if (!QuestTag.IsValid())
		{
			continue;
		}

		if (QuestStateEntry.QuestState == ENetherCrownQuestState::Rewarded)
		{
			continue;
		}

		const UNetherCrownQuestData* QuestData{ FNetherCrownUtilManager::GetQuestDataAssetByGameplayTag(QuestTag) };
		if (!ensureAlways(IsValid(QuestData)))
		{
			continue;
		}

		QuestEntryDescriptions.Add(QuestData->GetQuestDescription());
	}

	return QuestEntryDescriptions;
}
