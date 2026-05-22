// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownQuestCondition.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownQuestComponent.h"

bool UNetherCrownQuestCondition::IsConditionSatisfied(const ANetherCrownCharacter* QuestOwner, const FGameplayTag& QuestTag) const
{
	return false;
}

bool UNetherCrownQuestCondition::IsConditionTargetEqual(const FGameplayTag& TargetTag) const
{
	return false;
}

bool UNetherCrownQuestMonsterKillCondition::IsConditionSatisfied(const ANetherCrownCharacter* QuestOwner, const FGameplayTag& QuestTag) const
{
	const UNetherCrownQuestComponent* QuestComponent{ QuestOwner->GetQuestComponent() };
	if (!ensureAlways(IsValid(QuestComponent)))
	{
		return false;
	}

	const int32 CurrentQuestCount{ QuestComponent->GetQuestCountProgress(QuestTag, RequiredQuestTargetTag) };
	return CurrentQuestCount >= RequiredKillCount;
}

bool UNetherCrownQuestMonsterKillCondition::IsConditionTargetEqual(const FGameplayTag& TargetTag) const
{
	return TargetTag == RequiredQuestTargetTag;
}

bool UNetherCrownQuestFindItemCondition::IsConditionSatisfied(const ANetherCrownCharacter* QuestOwner, const FGameplayTag& QuestTag) const
{
	const UNetherCrownQuestComponent* QuestComponent{ QuestOwner->GetQuestComponent() };
	if (!ensureAlways(IsValid(QuestComponent)))
	{
		return false;
	}

	const int32 CurrentQuestItemCount{ QuestComponent->GetQuestCountProgress(QuestTag, RequiredQuestTargetTag) };
	return CurrentQuestItemCount >= RequiredQuestItemCount;
}
