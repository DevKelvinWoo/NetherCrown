// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
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
	const TArray<FText>& GetQuestDialogues() const { return QuestDialogues; }
	const TArray<FText>& GetQuestInprogressDialogues() const { return QuestInprogressDialogues; }
	const TArray<FText>& GetQuestEndDialogues() const { return QuestEndDialogues; }

	const FGameplayTag& GetQuestTag() const { return QuestTag; }
	const UNetherCrownQuestCondition* GetQuestCondition() const { return QuestCondition; }
	const UNetherCrownQuestReward* GetQuestReward() const { return QuestReward; }

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

USTRUCT(BlueprintType)
struct FNetherCrownQuestDataTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	const FGameplayTag& GetQuestTag() const { return QuestTag; }
	TSoftObjectPtr<UNetherCrownQuestData> GetQuestDataAsset() const { return QuestDataAssetSoft; }

private:
	UPROPERTY(EditAnywhere, Category = "QuestTag")
	FGameplayTag QuestTag{};

	UPROPERTY(EditAnywhere, Category = "QuestData")
	TSoftObjectPtr<UNetherCrownQuestData> QuestDataAssetSoft{};
};
