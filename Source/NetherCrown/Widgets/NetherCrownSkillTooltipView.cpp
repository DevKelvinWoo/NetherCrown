// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownSkillTooltipView.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UNetherCrownSkillTooltipView::SetSkillTooltipText(const FText& Text)
{
	if (!ensureAlways(NativeSkillToolTipText))
	{
		return;
	}

	NativeSkillToolTipText->SetText(Text);
}

void UNetherCrownSkillTooltipView::SetSkillThumbnailImage(const TSoftObjectPtr<UTexture2D> SoftTexture)
{
	if (!ensureAlways(NativeSkillThumbnailImage))
	{
		return;
	}

	NativeSkillThumbnailImage->SetBrushFromSoftTexture(SoftTexture);
}
