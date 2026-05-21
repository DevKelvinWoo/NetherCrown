// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownNPCDialogueWidgetView.h"

#include "InputCoreTypes.h"
#include "Components/TextBlock.h"
#include "ViewModel/NetherCrownNPCDialogueWidgetViewModel.h"

void UNetherCrownNPCDialogueWidgetView::InitViewModel(ANetherCrownCharacter* InModelCharacter, const FGameplayTag& InQuestTag)
{
	if (!ensureAlways(IsValid(NPCDialogueWidgetViewModel)))
	{
		return;
	}

	NPCDialogueWidgetViewModel->InitViewModel(InModelCharacter, InQuestTag);
}

void UNetherCrownNPCDialogueWidgetView::SetDialogueText(const TArray<FText>& InDialogue, const int32 DialogueIndex)
{
	if (!ensureAlways(IsValid(DialogueText)))
	{
		return;
	}

	DialogueArray = InDialogue;

	if (DialogueArray.IsValidIndex(DialogueIndex))
	{
		CurrentDialogueIndex = DialogueIndex;
		DialogueText->SetText(DialogueArray[DialogueIndex]);
	}
}

void UNetherCrownNPCDialogueWidgetView::NativeOnInitialized()
{
	NPCDialogueWidgetViewModel = NewObject<UNetherCrownNPCDialogueWidgetViewModel>(this);
	if (!ensureAlways(IsValid(NPCDialogueWidgetViewModel)))
	{
		return;
	}

	Super::NativeOnInitialized();
}

void UNetherCrownNPCDialogueWidgetView::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
	SetKeyboardFocus();
}

void UNetherCrownNPCDialogueWidgetView::NativeDestruct()
{
	if (ensureAlways(IsValid(NPCDialogueWidgetViewModel)))
	{
		NPCDialogueWidgetViewModel->ResetViewModel();
	}

	Super::NativeDestruct();
}

FReply UNetherCrownNPCDialogueWidgetView::NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	Super::NativeOnKeyDown(MyGeometry, InKeyEvent);

	if (InKeyEvent.GetKey() != EKeys::Y)
	{
		return FReply::Unhandled();
	}

	const int32 NextDialogueIndex{ CurrentDialogueIndex + 1 };
	if (DialogueArray.IsValidIndex(NextDialogueIndex))
	{
		SetDialogueText(DialogueArray, NextDialogueIndex);
		return FReply::Handled();
	}

	if (!ensureAlways(IsValid(NPCDialogueWidgetViewModel)))
	{
		return FReply::Handled();
	}

	HideScreen();
	NPCDialogueWidgetViewModel->RequestAcceptQuestState();

	return FReply::Handled();
}
