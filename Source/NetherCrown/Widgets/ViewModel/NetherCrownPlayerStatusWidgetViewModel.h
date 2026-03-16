// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "NetherCrownPlayerStatusWidgetViewModel.generated.h"

class ANetherCrownCharacter;
enum class ENetherCrownSkillKeyEnum : uint8;

UCLASS()
class NETHERCROWN_API UNetherCrownPlayerStatusWidgetViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnChacterMPModified, const float);
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSkillCoolDownModified, const float, const ENetherCrownSkillKeyEnum);

public:
	void InitWidget(ANetherCrownCharacter* InModelCharacter);

	FOnChacterMPModified& GetOnCharacterMPModified() { return OnCharacterMPModified; }
	FOnSkillCoolDownModified& GetOnSkillCoolDownModified() { return OnSkillCoolDownModified; }

private:
	void HandleOnCharacterMPModified(const float RemainMPRatio);
	void HandleOnSkillCoolDownModified(const float CoolDownRatio, const ENetherCrownSkillKeyEnum SkillKeyEnum);

	UPROPERTY(Transient)
	TWeakObjectPtr<ANetherCrownCharacter> ModelCharacterWeak{};

	FOnChacterMPModified OnCharacterMPModified;
	FOnSkillCoolDownModified OnSkillCoolDownModified;
};
