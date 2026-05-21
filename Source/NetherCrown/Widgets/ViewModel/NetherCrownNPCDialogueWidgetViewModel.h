// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MVVMViewModelBase.h"
#include "NetherCrownNPCDialogueWidgetViewModel.generated.h"

class ANetherCrownCharacter;

UCLASS()
class NETHERCROWN_API UNetherCrownNPCDialogueWidgetViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void InitViewModel(ANetherCrownCharacter* InModelCharacter, const FGameplayTag& InQuestTag);
	void ResetViewModel();

	void RequestAcceptQuestStateInProgress();
	bool IsQuestDone() const;

	void RewardToInteractCharacter();

private:
	UPROPERTY(Transient)
	TWeakObjectPtr<ANetherCrownCharacter> ModelCharacterWeak{};

	UPROPERTY(Transient)
	FGameplayTag QuestTag{};
};
