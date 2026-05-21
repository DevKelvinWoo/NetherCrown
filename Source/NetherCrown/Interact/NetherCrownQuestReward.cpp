// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownQuestReward.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"

bool UNetherCrownQuestReward::GrantReward(ANetherCrownCharacter* QuestOwner) const
{
	return IsValid(QuestOwner);
}

bool UNetherCrownQuestWeaponReward::GrantReward(ANetherCrownCharacter* QuestOwner) const
{
	//@TODO : 무기 보상 데이터가 구현되면 QuestOwner의 장비/인벤토리에 보상을 지급한다.
	UE_LOG(LogTemp, Warning, TEXT("Weapon Reward!"));
	return Super::GrantReward(QuestOwner);
}
