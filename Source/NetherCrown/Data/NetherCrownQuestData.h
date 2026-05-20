// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "NetherCrownQuestData.generated.h"

class UNetherCrownQuestReward;
class UNetherCrownQuestCondition;

UCLASS()
class NETHERCROWN_API UNetherCrownQuestData : public UDataAsset
{
	GENERATED_BODY()

public:

private:
	UPROPERTY(EditDefaultsOnly, Category = "Tag")
	FGameplayTag QuestTag{};

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Condition")
	TObjectPtr<UNetherCrownQuestCondition> QuestCondition{};

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Condition")
	TObjectPtr<UNetherCrownQuestReward> QuestReward{};

	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
	TArray<FText> QuestDialogues{};

	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
	TArray<FText> QuestInprogressDialogues{};

	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
	TArray<FText> QuestEndDialogues{};
};
