// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownNPCDialogueWidgetView.h"

#include "Components/TextBlock.h"

void UNetherCrownNPCDialogueWidgetView::SetDialogueText(const TArray<FText>& InDialogue, const int32 DialogueIndex)
{
	if (!ensureAlways(IsValid(DialogueText)))
	{
		return;
	}

	DialogueArray = InDialogue;

	if (DialogueArray.IsValidIndex(DialogueIndex))
	{
		DialogueText->SetText(DialogueArray[DialogueIndex]);
	}
}

void UNetherCrownNPCDialogueWidgetView::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
	SetKeyboardFocus();
}

FReply UNetherCrownNPCDialogueWidgetView::NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	Super::NativeOnKeyDown(MyGeometry, InKeyEvent);

	if (InKeyEvent.GetKey() != EKeys::Y)
	{
		return FReply::Unhandled();
	}

	++CurrentDialogueIndex;
	SetDialogueText(DialogueArray, CurrentDialogueIndex);

	return FReply::Handled();
}
