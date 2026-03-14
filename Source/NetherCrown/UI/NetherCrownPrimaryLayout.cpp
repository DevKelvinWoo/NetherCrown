// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownPrimaryLayout.h"

#include "NetherCrownUIScreenBase.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "NetherCrown/NetherCrown.h"

void UNetherCrownPrimaryLayout::AddScreenWidget(UNetherCrownUIScreenBase* ScreenWidget, const FGameplayTag& LayerTag)
{
	if (!IsValid(ScreenWidget))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("ScreenWidget is invalid in %hs"), __FUNCTION__);
		return;
	}

	UCanvasPanel* LayerPanel{ GetLayerPanel(LayerTag) };
	if (!ensureAlways(IsValid(LayerPanel)))
	{
		return;
	}

	if (ScreenWidget->GetParent())
	{
		ScreenWidget->RemoveFromParent();
	}

	UCanvasPanelSlot* CanvasPanelSlot{ LayerPanel->AddChildToCanvas(ScreenWidget) };
	if (!ensureAlways(IsValid(CanvasPanelSlot)))
	{
		return;
	}

	CanvasPanelSlot->SetAnchors(FAnchors{ 0.f, 0.f, 1.f, 1.f });
	CanvasPanelSlot->SetOffsets(FMargin{ 0.f });
}

void UNetherCrownPrimaryLayout::RemoveScreenWidget(UNetherCrownUIScreenBase* ScreenWidget)
{
	if (!IsValid(ScreenWidget))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("ScreenWidget is invalid in %hs"), __FUNCTION__);
		return;
	}

	ScreenWidget->RemoveFromParent();
}

UCanvasPanel* UNetherCrownPrimaryLayout::GetLayerPanel(const FGameplayTag& LayerTag) const
{
	if (LayerTag == NetherCrownTags::UI_Layer_Game)
	{
		return NativeGameLayerPanel;
	}

	if (LayerTag == NetherCrownTags::UI_Layer_GameMenu)
	{
		return NativeGameMenuLayerPanel;
	}

	if (LayerTag == NetherCrownTags::UI_Layer_Menu)
	{
		return NativeMenuLayerPanel;
	}

	if (LayerTag == NetherCrownTags::UI_Layer_Modal)
	{
		return NativeModalLayerPanel;
	}

	return nullptr;
}
