// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownPlayerStatusWidgetViewModel.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownPlayerStatComponent.h"
#include "NetherCrown/PlayerState/NetherCrownPlayerState.h"

void UNetherCrownPlayerStatusWidgetViewModel::InitWidget(ANetherCrownCharacter* InModelCharacter)
{
	if (!ensureAlways(IsValid(InModelCharacter)))
	{
		return;
	}

	ModelCharacterWeak = MakeWeakObjectPtr(InModelCharacter);

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

void UNetherCrownPlayerStatusWidgetViewModel::HandleOnCharacterMPModified(const float RemainMPRatio)
{
	if (OnCharacterMPModified.IsBound())
	{
		OnCharacterMPModified.Broadcast(RemainMPRatio);
	}
}
