// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrown/UI/NetherCrownUIScreenBase.h"
#include "NetherCrownPlayerStatusWidgetView.generated.h"

class UNetherCrownSliderComponent;
class UNetherCrownPlayerStatusWidgetViewModel;

UCLASS()
class NETHERCROWN_API UNetherCrownPlayerStatusWidgetView : public UNetherCrownUIScreenBase
{
	GENERATED_BODY()

public:
	void HandleOnRepPlayerState();

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UNetherCrownSliderComponent> NativeHealthBar{};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UNetherCrownSliderComponent> NativeMPBar{};

private:
	void ApplyMPSlider(const float RemainMPRatio);
	void InitViewModel();

	UPROPERTY(Transient)
	TObjectPtr<UNetherCrownPlayerStatusWidgetViewModel> PlayerStatusWidgetViewModel{};
};
