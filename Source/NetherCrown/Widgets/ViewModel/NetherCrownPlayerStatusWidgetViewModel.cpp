// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownPlayerStatusWidgetViewModel.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownPlayerStatComponent.h"
#include "NetherCrown/Components/NetherCrownSkillComponent.h"
#include "NetherCrown/PlayerState/NetherCrownPlayerState.h"
#include "NetherCrown/Skill/NetherCrownSkillObject.h"

void UNetherCrownPlayerStatusWidgetViewModel::InitViewModel(ANetherCrownCharacter* InModelCharacter)
{
	if (!ensureAlways(IsValid(InModelCharacter)))
	{
		return;
	}

	UnbindModelDelegates();

	ModelCharacterWeak = MakeWeakObjectPtr(InModelCharacter);

	UNetherCrownSkillComponent* SkillComponent{ InModelCharacter->GetSkillComponent() };
	if (ensureAlways(IsValid(SkillComponent)))
	{
		BoundSkillComponentWeak = MakeWeakObjectPtr(SkillComponent);
		SkillComponent->GetOnSkillCoolDownModified().AddUObject(this, &ThisClass::HandleOnSkillCoolDownModified);
	}

	const ANetherCrownPlayerState* OwnerPlayerState{ Cast<ANetherCrownPlayerState>(InModelCharacter->GetPlayerState()) };
	if (!ensureAlways(IsValid(OwnerPlayerState)))
	{
		return;
	}

	UNetherCrownPlayerStatComponent* OwnerCharacterStatComponent{ OwnerPlayerState->GetNetherCrownPlayerStatComponent() };
	if (!ensureAlways(IsValid(OwnerCharacterStatComponent)))
	{
		return;
	}

	BoundPlayerStatComponentWeak = MakeWeakObjectPtr(OwnerCharacterStatComponent);
	OwnerCharacterStatComponent->GetOnCharacterHPModified().AddUObject(this, &ThisClass::HandleOnCharacterHPModified);
	OwnerCharacterStatComponent->GetOnCharacterMPModified().AddUObject(this, &ThisClass::HandleOnCharacterMPModified);

	const FNetherCrownPlayerStat& PlayerStatData{ OwnerCharacterStatComponent->GetPlayerStatData() };
	const float RemainHPRatio{ PlayerStatData.CharacterMaxHP > 0 ? (PlayerStatData.CharacterHP) / (PlayerStatData.CharacterMaxHP) : 0.0f };
	const float RemainMPRatio{ PlayerStatData.CharacterMaxMP > 0 ? (PlayerStatData.CharacterMP) / (PlayerStatData.CharacterMaxMP) : 0.0f };
	HandleOnCharacterHPModified(RemainHPRatio);
	HandleOnCharacterMPModified(RemainMPRatio);
}

void UNetherCrownPlayerStatusWidgetViewModel::ResetViewModel()
{
	UnbindModelDelegates();
	ModelCharacterWeak.Reset();
}

UNetherCrownSkillObject* UNetherCrownPlayerStatusWidgetViewModel::GetSkillObject(const ENetherCrownSkillKeyEnum SkillKeyEnum) const
{
	const ANetherCrownCharacter* ModelCharacter{ ModelCharacterWeak.Get() };
	if (!ensureAlways(IsValid(ModelCharacter)))
	{
		return nullptr;
	}

	UNetherCrownSkillComponent* SkillComponent{ ModelCharacter->GetSkillComponent() };
	if (!ensureAlways(IsValid(SkillComponent)))
	{
		return nullptr;
	}

	return SkillComponent->GetSkillObject(SkillKeyEnum);
}

void UNetherCrownPlayerStatusWidgetViewModel::UnbindModelDelegates()
{
	if (UNetherCrownPlayerStatComponent* BoundPlayerStatComponent{ BoundPlayerStatComponentWeak.Get() })
	{
		BoundPlayerStatComponent->GetOnCharacterHPModified().RemoveAll(this);
		BoundPlayerStatComponent->GetOnCharacterMPModified().RemoveAll(this);
	}

	if (UNetherCrownSkillComponent* BoundSkillComponent{ BoundSkillComponentWeak.Get() })
	{
		BoundSkillComponent->GetOnSkillCoolDownModified().RemoveAll(this);
	}

	BoundPlayerStatComponentWeak.Reset();
	BoundSkillComponentWeak.Reset();
}

void UNetherCrownPlayerStatusWidgetViewModel::HandleOnCharacterHPModified(const float RemainHPRatio)
{
	if (OnCharacterHPModified.IsBound())
	{
		OnCharacterHPModified.Broadcast(RemainHPRatio);
	}
}

void UNetherCrownPlayerStatusWidgetViewModel::HandleOnCharacterMPModified(const float RemainMPRatio)
{
	if (OnCharacterMPModified.IsBound())
	{
		OnCharacterMPModified.Broadcast(RemainMPRatio);
	}
}

void UNetherCrownPlayerStatusWidgetViewModel::HandleOnSkillCoolDownModified(const float CoolDownRatio, const ENetherCrownSkillKeyEnum SkillKeyEnum)
{
	if (OnSkillCoolDownModified.IsBound())
	{
		OnSkillCoolDownModified.Broadcast(CoolDownRatio, SkillKeyEnum);
	}
}
