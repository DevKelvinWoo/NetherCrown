// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrown/UI/NetherCrownUIScreenBase.h"
#include "NetherCrownPlayerStatusWidgetView.generated.h"

class UNetherCrownSliderComponent;
class UNetherCrownSkillIconComponent;
class UNetherCrownPlayerStatusWidgetViewModel;
enum class ENetherCrownSkillKeyEnum : uint8;

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

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UNetherCrownSkillIconComponent> NativeQSkillIcon{};
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UNetherCrownSkillIconComponent> NativeESkillIcon{};
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UNetherCrownSkillIconComponent> NativeRSkillIcon{};
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UNetherCrownSkillIconComponent> NativeShiftSkillIcon{};

private:
	void ApplyMPSlider(const float RemainMPRatio);
	void ApplySkillCoolDownSlider(const float CoolDownRatio, const ENetherCrownSkillKeyEnum SkillKeyEnum);

	void InitViewModel();

	UPROPERTY(Transient)
	TObjectPtr<UNetherCrownPlayerStatusWidgetViewModel> PlayerStatusWidgetViewModel{};
};
