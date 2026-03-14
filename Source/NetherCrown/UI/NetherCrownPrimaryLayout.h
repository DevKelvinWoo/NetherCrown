// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "NetherCrownPrimaryLayout.generated.h"

class UCanvasPanel;
class UNetherCrownUIScreenBase;

UCLASS(Blueprintable)
class NETHERCROWN_API UNetherCrownPrimaryLayout : public UUserWidget
{
	GENERATED_BODY()

public:
	void AddScreenWidget(UNetherCrownUIScreenBase* ScreenWidget, const FGameplayTag& LayerTag);
	void RemoveScreenWidget(UNetherCrownUIScreenBase* ScreenWidget);

private:
	UCanvasPanel* GetLayerPanel(const FGameplayTag& LayerTag) const;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> NativeGameLayerPanel{};

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> NativeGameMenuLayerPanel{};

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> NativeMenuLayerPanel{};

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> NativeModalLayerPanel{};
};
