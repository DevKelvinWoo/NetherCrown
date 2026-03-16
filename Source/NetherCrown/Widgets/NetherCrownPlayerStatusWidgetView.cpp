// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownPlayerStatusWidgetView.h"

#include "NetherCrownSliderComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "ViewModel/NetherCrownPlayerStatusWidgetViewModel.h"

void UNetherCrownPlayerStatusWidgetView::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayerStatusWidgetViewModel = NewObject<UNetherCrownPlayerStatusWidgetViewModel>(this);
	if (!ensureAlways(IsValid(PlayerStatusWidgetViewModel)))
	{
		return;
	}

	PlayerStatusWidgetViewModel->GetOnCharacterMPModified().AddUObject(this, &ThisClass::ApplyMPSlider);

	ANetherCrownCharacter* OwningCharacter{ GetOwningNetherCrownCharacter() };
	if (ensureAlways(IsValid(OwningCharacter)))
	{
		OwningCharacter->GetOnRepPlayerState().AddUObject(this, &ThisClass::HandleOnRepPlayerState);
	}
}

void UNetherCrownPlayerStatusWidgetView::NativeDestruct()
{
	ANetherCrownCharacter* OwningCharacter{ GetOwningNetherCrownCharacter() };
	if (ensureAlways(IsValid(OwningCharacter)))
	{
		OwningCharacter->GetOnRepPlayerState().RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UNetherCrownPlayerStatusWidgetView::HandleOnRepPlayerState()
{
	InitViewModel();
}

void UNetherCrownPlayerStatusWidgetView::ApplyMPSlider(const float RemainMPRatio)
{
	if (!ensureAlways(IsValid(NativeMPBar)))
	{
		return;
	}

	NativeMPBar->SetProgress(RemainMPRatio);
}

void UNetherCrownPlayerStatusWidgetView::InitViewModel()
{
	if (!ensureAlways(IsValid(PlayerStatusWidgetViewModel)))
	{
		return;
	}

	ANetherCrownCharacter* OwningCharacter{ GetOwningNetherCrownCharacter() };
	if (!ensureAlways(IsValid(OwningCharacter)))
	{
		return;
	}

	PlayerStatusWidgetViewModel->InitWidget(OwningCharacter);
}
