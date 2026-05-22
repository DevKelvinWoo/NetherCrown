// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NetherCrown/Components/NetherCrownQuestComponent.h"
#include "UObject/Object.h"
#include "NetherCrownQuestEntryObject.generated.h"

UCLASS()
class NETHERCROWN_API UNetherCrownQuestEntryObject : public UObject
{
	GENERATED_BODY()

public:
	const FText& GetQuestDescriptionText() const { return QuestDescriptionText; }
	bool ShouldPlayCompleteAnimation() const { return bShouldPlayCompleteAnimation; }

	void SetQuestDescriptionText(const FText& InQuestDescriptionText) { QuestDescriptionText = InQuestDescriptionText; }
	void SetShouldPlayCompleteAnimation(const bool bInShouldPlayCompleteAnimation) { bShouldPlayCompleteAnimation = bInShouldPlayCompleteAnimation; }

private:
	UPROPERTY(Transient)
	FText QuestDescriptionText{};

	UPROPERTY(Transient)
	bool bShouldPlayCompleteAnimation{ false };
};
