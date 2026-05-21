// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownQuestManagerSubsystem.h"

#include "NetherCrown/NetherCrown.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownQuestComponent.h"
#include "NetherCrown/Data/NetherCrownQuestData.h"
#include "NetherCrown/Interact/NetherCrownQuestCondition.h"
#include "NetherCrown/Interact/NetherCrownQuestReward.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

UNetherCrownQuestData* UNetherCrownQuestManagerSubsystem::GetQuestDataByGameplayTag(const FGameplayTag& QuestTag) const
{
	if (!QuestTag.IsValid())
	{
		return nullptr;
	}

	return FNetherCrownUtilManager::GetQuestDataAssetByGameplayTag(QuestTag);
}

bool UNetherCrownQuestManagerSubsystem::CanCompleteQuest(const ANetherCrownCharacter* QuestOwner, const FGameplayTag& QuestTag) const
{
	if (!ensureAlways(IsValid(QuestOwner)) || !QuestOwner->HasAuthority())
	{
		return false;
	}

	const UNetherCrownQuestData* QuestData{ GetQuestDataByGameplayTag(QuestTag) };
	if (!ensureAlways(IsValid(QuestData)))
	{
		return false;
	}

	const UNetherCrownQuestCondition* QuestCondition{ QuestData->GetQuestCondition() };
	if (!ensureAlways(IsValid(QuestCondition)))
	{
		return false;
	}

	UE_LOG(LogNetherCrown, Log, TEXT("Check QuestCondition Class: %s"), *QuestCondition->GetClass()->GetName());

	return QuestCondition->IsConditionSatisfied(QuestOwner, QuestTag);
}

bool UNetherCrownQuestManagerSubsystem::TryGrantQuestReward(ANetherCrownCharacter* QuestOwner, const FGameplayTag& QuestTag) const
{
	if (!ensureAlways(IsValid(QuestOwner)) || !QuestOwner->HasAuthority())
	{
		return false;
	}

	const UNetherCrownQuestData* QuestData{ GetQuestDataByGameplayTag(QuestTag) };
	if (!ensureAlways(IsValid(QuestData)))
	{
		return false;
	}

	const UNetherCrownQuestReward* QuestReward{ QuestData->GetQuestReward() };
	if (!ensureAlways(IsValid(QuestReward)))
	{
		return false;
	}

	UE_LOG(LogNetherCrown, Log, TEXT("Grant QuestReward Class: %s"), *QuestReward->GetClass()->GetName());

	return QuestReward->GrantReward(QuestOwner);
}

bool UNetherCrownQuestManagerSubsystem::TryCompleteQuest(ANetherCrownCharacter* QuestOwner, const FGameplayTag& QuestTag) const
{
	if (!ensureAlways(IsValid(QuestOwner)) || !QuestOwner->HasAuthority())
	{
		return false;
	}

	UNetherCrownQuestComponent* QuestComponent{ QuestOwner->GetQuestComponent() };
	if (!ensureAlways(IsValid(QuestComponent)))
	{
		return false;
	}

	if (QuestComponent->GetQuestState(QuestTag) == ENetherCrownQuestState::Done)
	{
		return true;
	}

	if (!CanCompleteQuest(QuestOwner, QuestTag))
	{
		return false;
	}

	QuestComponent->RequestAcceptQuestState(QuestTag, ENetherCrownQuestState::Done);
	return true;
}
