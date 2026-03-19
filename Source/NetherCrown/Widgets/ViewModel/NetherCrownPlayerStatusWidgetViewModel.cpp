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

	ModelCharacterWeak = MakeWeakObjectPtr(InModelCharacter);

	UNetherCrownSkillComponent* SkillComponent{ InModelCharacter->GetSkillComponent() };
	if (ensureAlways(IsValid(SkillComponent)))
	{
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

	OwnerCharacterStatComponent->GetOnCharacterMPModified().AddUObject(this, &ThisClass::HandleOnCharacterMPModified);

	const FNetherCrownPlayerStatData& PlayerStatData{ OwnerCharacterStatComponent->GetPlayerStatData() };
	const float RemainMPRatio{ PlayerStatData.CharacterMaxMP > 0 ? (PlayerStatData.CharacterMP) / (PlayerStatData.CharacterMaxMP) : 0.0f };
	HandleOnCharacterMPModified(RemainMPRatio);
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
