// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NetherCrownQuestData.h"
#include "Engine/DataAsset.h"
#include "NetherCrownNPCData.generated.h"

UCLASS()
class NETHERCROWN_API UNetherCrownNPCDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const TArray<TObjectPtr<UNetherCrownQuestData>>& GetQuestDataArray() const { return QuestDataArray; }
	const FText& GetNonQuestNPCDialogues() const { return NonQuestNPCDialogues; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Tag")
	FGameplayTag NPCTag{};

	UPROPERTY(EditDefaultsOnly, Category = "Setting")
	bool bIsQuestNPC{ false };

	UPROPERTY(EditDefaultsOnly, Category = "QuestData")
	TArray<TObjectPtr<UNetherCrownQuestData>> QuestDataArray{};

	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
	FText NonQuestNPCDialogues{};
};
