// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrown/UI/NetherCrownUIScreenBase.h"
#include "NetherCrownPlayerStatusWidgetView.generated.h"

class UNetherCrownSliderComponent;
class UNetherCrownSkillIconComponent;
class UNetherCrownPlayerStatusWidgetViewModel;
class ANetherCrownCharacter;
enum class ENetherCrownSkillKeyEnum : uint8;

UCLASS()
class NETHERCROWN_API UNetherCrownPlayerStatusWidgetView : public UNetherCrownUIScreenBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;
	virtual void HandleOwningCharacterChanged(ANetherCrownCharacter* NewOwningCharacter) override;

private:
	void UnbindCharacterDelegates();
	void ApplyHealthSlider(const float RemainHPRatio);
	void ApplyMPSlider(const float RemainMPRatio);
	void ApplySkillCoolDownSlider(const float CoolDownRatio, const ENetherCrownSkillKeyEnum SkillKeyEnum);
	void HandleOnEquipWeapon(const bool bEquipWeapon);
	void SetSkillIconsActive(const bool bActive, const bool bPlayActivateAnimation);

	void InitViewModel();

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UNetherCrownSliderComponent> NativeHealthBar{};

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UNetherCrownSliderComponent> NativeMPBar{};

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UNetherCrownSkillIconComponent> NativeQSkillIcon{};
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UNetherCrownSkillIconComponent> NativeESkillIcon{};
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UNetherCrownSkillIconComponent> NativeRSkillIcon{};
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UNetherCrownSkillIconComponent> NativeShiftSkillIcon{};

	UPROPERTY(Transient)
	TObjectPtr<UNetherCrownPlayerStatusWidgetViewModel> PlayerStatusWidgetViewModel{};

	TWeakObjectPtr<ANetherCrownCharacter> BoundCharacterWeak{};
};
