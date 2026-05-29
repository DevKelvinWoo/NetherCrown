// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "NetherCrownPlayerStatusWidgetViewModel.generated.h"

class ANetherCrownCharacter;
class UNetherCrownPlayerStatComponent;
class UNetherCrownSkillComponent;
class UNetherCrownSkillObject;
enum class ENetherCrownSkillKeyEnum : uint8;

UCLASS()
class NETHERCROWN_API UNetherCrownPlayerStatusWidgetViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterHPModified, const float);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterMPModified, const float);
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSkillCoolDownModified, const float, const ENetherCrownSkillKeyEnum);

public:
	void InitViewModel(ANetherCrownCharacter* InModelCharacter);
	void ResetViewModel();

	UNetherCrownSkillObject* GetSkillObject(const ENetherCrownSkillKeyEnum SkillKeyEnum) const;

	FOnCharacterHPModified& GetOnCharacterHPModified() { return OnCharacterHPModified; }
	FOnCharacterMPModified& GetOnCharacterMPModified() { return OnCharacterMPModified; }
	FOnSkillCoolDownModified& GetOnSkillCoolDownModified() { return OnSkillCoolDownModified; }

private:
	void UnbindModelDelegates();
	void HandleOnCharacterHPModified(const float RemainHPRatio);
	void HandleOnCharacterMPModified(const float RemainMPRatio);
	void HandleOnSkillCoolDownModified(const float CoolDownRatio, const ENetherCrownSkillKeyEnum SkillKeyEnum);

	UPROPERTY(Transient)
	TWeakObjectPtr<ANetherCrownCharacter> ModelCharacterWeak{};

	UPROPERTY(Transient)
	TWeakObjectPtr<UNetherCrownPlayerStatComponent> BoundPlayerStatComponentWeak{};

	UPROPERTY(Transient)
	TWeakObjectPtr<UNetherCrownSkillComponent> BoundSkillComponentWeak{};

	FOnCharacterHPModified OnCharacterHPModified;
	FOnCharacterMPModified OnCharacterMPModified;
	FOnSkillCoolDownModified OnSkillCoolDownModified;
};
