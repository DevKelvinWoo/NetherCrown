// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownInteractWidgetView.h"

#include "Components/Border.h"

void UNetherCrownInteractWidgetView::SetInteractWidgetBorderTexture(UTexture2D* InTexture)
{
	if (!ensureAlways(IsValid(InteractWidgetBorder)))
	{
		return;
	}

	InteractWidgetBorder->SetBrushFromTexture(InTexture);
}
