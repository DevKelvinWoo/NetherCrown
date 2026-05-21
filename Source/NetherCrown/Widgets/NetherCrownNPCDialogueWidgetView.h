// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NetherCrown/UI/NetherCrownUIScreenBase.h"
#include "NetherCrownNPCDialogueWidgetView.generated.h"

class ANetherCrownCharacter;
class UBorder;
class UTextBlock;
class UNetherCrownNPCDialogueWidgetViewModel;

UCLASS()
class NETHERCROWN_API UNetherCrownNPCDialogueWidgetView : public UNetherCrownUIScreenBase
{
	GENERATED_BODY()

public:
	void SetDialogueText(const TArray<FText>& InDialogue, const int32 DialogueIndex);
	void InitViewModel(ANetherCrownCharacter* InModelCharacter, const FGameplayTag& InQuestTag);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

private:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UBorder> DialogueBorder{};

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> DialogueText{};

	UPROPERTY(Transient)
	TArray<FText> DialogueArray{};

	UPROPERTY(Transient)
	TObjectPtr<UNetherCrownNPCDialogueWidgetViewModel> NPCDialogueWidgetViewModel{};

	int32 CurrentDialogueIndex{};
};
