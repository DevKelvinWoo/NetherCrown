// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownSkillIconComponent.h"

#include "NetherCrownSkillTooltipView.h"

#include "Components/MenuAnchor.h"
#include "Components/ProgressBar.h"

void UNetherCrownSkillIconComponent::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensureAlways(NativeSkillTooltipAnchor))
	{
		NativeSkillTooltipAnchor->OnGetUserMenuContentEvent.BindUFunction(this, TEXT("HandleOnGetSkillTooltipWidgetInMenuAnchor"));
	}
}

void UNetherCrownSkillIconComponent::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (ensureAlways(NativeSkillTooltipAnchor) && !(NativeSkillTooltipAnchor->IsOpen()))
	{
		NativeSkillTooltipAnchor->Open(false);
	}
}

void UNetherCrownSkillIconComponent::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (ensureAlways(NativeSkillTooltipAnchor) && NativeSkillTooltipAnchor->IsOpen())
	{
		NativeSkillTooltipAnchor->Close();
	}
}

UUserWidget* UNetherCrownSkillIconComponent::HandleOnGetSkillTooltipWidgetInMenuAnchor()
{
	if (!SkillTooltipWidgetClass)
	{
		return nullptr;
	}

	UNetherCrownSkillTooltipView* SkillTooltipWidget{ CreateWidget<UNetherCrownSkillTooltipView>(GetWorld(), SkillTooltipWidgetClass) };
	if (!ensureAlways(SkillTooltipWidget))
	{
		return nullptr;
	}

	SkillTooltipWidget->SetSkillThumbnailImage(SkillThumbnailTexture);
	SkillTooltipWidget->SetSkillTooltipText(SkillToolTipText);

	return SkillTooltipWidget;
}

void UNetherCrownSkillIconComponent::SetSkillCoolDownProgress(float Percent)
{
	if (!ensureAlways(NativeSkillThumbnailProgressBar))
	{
		return;
	}

	NativeSkillThumbnailProgressBar->SetPercent(Percent);
}
