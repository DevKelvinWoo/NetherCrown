// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownQuestCondition.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownQuestComponent.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"

bool UNetherCrownQuestCondition::IsConditionSatisfied(const ANetherCrownCharacter* QuestOwner) const
{
	return IsValid(QuestOwner);
}

bool UNetherCrownQuestMonsterKillCondition::IsConditionSatisfied(const ANetherCrownCharacter* QuestOwner) const
{
	//@TODO : 몬스터 처치 조건 데이터가 구현되면 QuestOwner의 진행도를 확인한다.
	return Super::IsConditionSatisfied(QuestOwner);
}

bool UNetherCrownQuestFindItemCondition::IsConditionSatisfied(const ANetherCrownCharacter* QuestOwner) const
{
	if (!Super::IsConditionSatisfied(QuestOwner))
	{
		return false;
	}

	const UNetherCrownQuestComponent* QuestComponent{ QuestOwner->GetQuestComponent() };
	if (!ensureAlways(IsValid(QuestComponent)))
	{
		return false;
	}

	const int32 CurrentWeaponSellQuestItemCount{ QuestComponent->GetQuestCountProgress(NetherCrownTags::Quest_WeaponSell) };
	return CurrentWeaponSellQuestItemCount >= RequiredWeaponSellQuestItemCount;
}
