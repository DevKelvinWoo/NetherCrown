// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NetherCrownQuestManagerSubsystem.generated.h"

class ANetherCrownCharacter;
class UNetherCrownQuestData;

UCLASS()
class NETHERCROWN_API UNetherCrownQuestManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UNetherCrownQuestData* GetQuestDataByGameplayTag(const FGameplayTag& QuestTag) const;

	bool CanCompleteQuest(const ANetherCrownCharacter* QuestOwner, const FGameplayTag& QuestTag) const;
	bool TryGrantQuestReward(ANetherCrownCharacter* QuestOwner, const FGameplayTag& QuestTag) const;
	bool TryCompleteQuest(ANetherCrownCharacter* QuestOwner, const FGameplayTag& QuestTag) const;
};
