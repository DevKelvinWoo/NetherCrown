// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownSkillIconComponent.h"

#include "NetherCrownSkillTooltipView.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownSkillComponent.h"
#include "Components/ProgressBar.h"
#include "Components/MenuAnchor.h"
#include "Components/TextBlock.h"

void UNetherCrownSkillIconComponent::SetHandlingSkillObject(UNetherCrownSkillObject* InHandlingSkillObject)
{
	HandlingSkillObject = InHandlingSkillObject;
	ApplySkillVisual();
}

void UNetherCrownSkillIconComponent::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	TryResolveHandlingSkillObject();
	ApplySkillVisual();

	if (ensureAlways(NativeSkillTooltipAnchor))
	{
		NativeSkillTooltipAnchor->OnGetUserMenuContentEvent.BindUFunction(this, TEXT("HandleOnGetSkillTooltipWidgetInMenuAnchor"));
	}
}

void UNetherCrownSkillIconComponent::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	TryResolveHandlingSkillObject();

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

	TryResolveHandlingSkillObject();

	if (!ensureAlways(IsValid(HandlingSkillObject)))
	{
		return nullptr;
	}

	UNetherCrownSkillTooltipView* SkillTooltipWidget{ CreateWidget<UNetherCrownSkillTooltipView>(GetWorld(), SkillTooltipWidgetClass) };
	if (!ensureAlways(IsValid(SkillTooltipWidget)))
	{
		return nullptr;
	}

	const FNetherCrownSkillData& SkillData{ HandlingSkillObject->GetSkillData() };
	SkillTooltipWidget->SetSkillThumbnailImage(SkillData.SkillIconTextureSoft);
	SkillTooltipWidget->SetSkillTooltipText(FText::FromString(SkillData.SkillDescription));

	return SkillTooltipWidget;
}

void UNetherCrownSkillIconComponent::SetSkillCoolDownProgress(float Percent)
{
	if (!ensureAlways(IsValid(NativeSkillThumbnailProgressBar)))
	{
		return;
	}

	NativeSkillThumbnailProgressBar->SetPercent(Percent);
}

void UNetherCrownSkillIconComponent::ApplySkillVisual()
{
	if (ensureAlways(IsValid(NativeSkillKeyText)))
	{
		NativeSkillKeyText->SetText(SkillKeyText);
	}

	TryResolveHandlingSkillObject();

	if (!IsValid(HandlingSkillObject))
	{
		return;
	}

	if (ensureAlways(IsValid(NativeSkillThumbnailProgressBar)))
	{
		const FNetherCrownSkillData& SkillData{ HandlingSkillObject->GetSkillData() };
		UTexture2D* SkillIconTexture{ SkillData.SkillIconTextureSoft.IsNull() ? nullptr : SkillData.SkillIconTextureSoft.LoadSynchronous() };
		if (!ensureAlways(IsValid(SkillIconTexture)))
		{
			return;
		}

		FProgressBarStyle ProgressBarStyle{ NativeSkillThumbnailProgressBar->GetWidgetStyle() };

		FSlateBrush FillBrush{ ProgressBarStyle.FillImage };
		FillBrush.SetResourceObject(SkillIconTexture);
		ProgressBarStyle.SetFillImage(FillBrush);

		FSlateBrush BackgroundBrush{ ProgressBarStyle.BackgroundImage };
		BackgroundBrush.SetResourceObject(SkillIconTexture);
		ProgressBarStyle.SetBackgroundImage(BackgroundBrush);

		NativeSkillThumbnailProgressBar->SetWidgetStyle(ProgressBarStyle);
	}
}

void UNetherCrownSkillIconComponent::TryResolveHandlingSkillObject()
{
	if (IsValid(HandlingSkillObject) || SkillKeyEnum == ENetherCrownSkillKeyEnum::None)
	{
		return;
	}

	ANetherCrownCharacter* OwningCharacter{ Cast<ANetherCrownCharacter>(GetOwningPlayerPawn()) };
	if (!ensureAlways(IsValid(OwningCharacter)))
	{
		return;
	}

	UNetherCrownSkillComponent* SkillComponent{ OwningCharacter->GetSkillComponent() };
	if (!ensureAlways(IsValid(SkillComponent)))
	{
		return;
	}

	HandlingSkillObject = SkillComponent->GetSkillObject(SkillKeyEnum);
}
