// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownNPCDialogueWidgetView.h"

void UNetherCrownNPCDialogueWidgetView::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
	SetKeyboardFocus();
}

FReply UNetherCrownNPCDialogueWidgetView::NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	Super::NativeOnKeyDown(MyGeometry, InKeyEvent);

	if (InKeyEvent.GetKey() != EKeys::LeftAlt)
	{
		return FReply::Unhandled();
	}

	UE_LOG(LogTemp, Warning, TEXT("Alt key pressed"));

	return FReply::Handled();
}
