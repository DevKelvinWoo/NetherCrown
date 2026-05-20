// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrown/UI/NetherCrownUIScreenBase.h"
#include "NetherCrownNPCDialogueWidgetView.generated.h"

class UBorder;
class UTextBlock;

UCLASS()
class NETHERCROWN_API UNetherCrownNPCDialogueWidgetView : public UNetherCrownUIScreenBase
{
	GENERATED_BODY()

public:
	void SetDialogueText(const TArray<FText>& InDialogue, const int32 DialogueIndex);

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

private:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UBorder> DialogueBorder{};

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> DialogueText{};

	UPROPERTY(Transient)
	TArray<FText> DialogueArray{};

	int32 CurrentDialogueIndex{};
};
