// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownNPCDialogueWidgetViewModel.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownQuestComponent.h"

void UNetherCrownNPCDialogueWidgetViewModel::InitViewModel(ANetherCrownCharacter* InModelCharacter, const FGameplayTag& InQuestTag)
{
	if (!ensureAlways(IsValid(InModelCharacter)))
	{
		return;
	}

	ModelCharacterWeak = MakeWeakObjectPtr(InModelCharacter);
	QuestTag = InQuestTag;
}

void UNetherCrownNPCDialogueWidgetViewModel::ResetViewModel()
{
	ModelCharacterWeak.Reset();
	QuestTag = FGameplayTag{};
}

void UNetherCrownNPCDialogueWidgetViewModel::RequestAcceptQuestStateInProgress()
{
	if (!ensureAlways(QuestTag.IsValid()))
	{
		return;
	}

	const ANetherCrownCharacter* ModelCharacter{ ModelCharacterWeak.Get() };
	if (!ensureAlways(IsValid(ModelCharacter)))
	{
		return;
	}

	UNetherCrownQuestComponent* QuestComponent{ ModelCharacter->GetQuestComponent() };
	if (!ensureAlways(IsValid(QuestComponent)))
	{
		return;
	}

	QuestComponent->RequestAcceptQuestState(QuestTag, ENetherCrownQuestState::InProgress);
}

bool UNetherCrownNPCDialogueWidgetViewModel::IsQuestDone() const
{
	if (!ensureAlways(QuestTag.IsValid()))
	{
		return false;
	}

	const ANetherCrownCharacter* ModelCharacter{ ModelCharacterWeak.Get() };
	if (!ensureAlways(IsValid(ModelCharacter)))
	{
		return false;
	}

	const UNetherCrownQuestComponent* QuestComponent{ ModelCharacter->GetQuestComponent() };
	if (!ensureAlways(IsValid(QuestComponent)))
	{
		return false;
	}

	return QuestComponent->GetQuestState(QuestTag) == ENetherCrownQuestState::Done;
}

void UNetherCrownNPCDialogueWidgetViewModel::RewardToInteractCharacter()
{
	if (!ensureAlways(QuestTag.IsValid()))
	{
		return;
	}

	const ANetherCrownCharacter* ModelCharacter{ ModelCharacterWeak.Get() };
	if (!ensureAlways(IsValid(ModelCharacter)))
	{
		return;
	}

	UNetherCrownQuestComponent* QuestComponent{ ModelCharacter->GetQuestComponent() };
	if (!ensureAlways(IsValid(QuestComponent)))
	{
		return;
	}

	QuestComponent->RequestGrantQuestReward(QuestTag);
}
