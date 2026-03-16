// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownPlayerStatComponent.h"

#include "Net/UnrealNetwork.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/PlayerState/NetherCrownPlayerState.h"

UNetherCrownPlayerStatComponent::UNetherCrownPlayerStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UNetherCrownPlayerStatComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerStatData);
}

void UNetherCrownPlayerStatComponent::OnRep_PlayerStatData(const FNetherCrownPlayerStatData& OldPlayerStatData)
{
	if (PlayerStatData.CharacterMP != OldPlayerStatData.CharacterMP)
	{
		OnCharacterMPModified.Broadcast(PlayerStatData.CharacterMP / PlayerStatData.CharacterMaxMP);
	}
}

void UNetherCrownPlayerStatComponent::AddPlayerShield(int32 InShieldValue)
{
	ANetherCrownPlayerState* OwnerPlayerState{ Cast<ANetherCrownPlayerState>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerPlayerState)))
	{
		return;
	}

	ANetherCrownCharacter* PlayerStateOwnerCharacter{ Cast<ANetherCrownCharacter>(OwnerPlayerState->GetPawn()) };
	if (!ensureAlways(IsValid(PlayerStateOwnerCharacter)) || !PlayerStateOwnerCharacter->HasAuthority())
	{
		return;
	}

	PlayerStatData.ShieldValue += InShieldValue;
}

void UNetherCrownPlayerStatComponent::ClearPlayerShield()
{
	ANetherCrownPlayerState* OwnerPlayerState{ Cast<ANetherCrownPlayerState>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerPlayerState)))
	{
		return;
	}

	ANetherCrownCharacter* PlayerStateOwnerCharacter{ Cast<ANetherCrownCharacter>(OwnerPlayerState->GetPawn()) };
	if (!ensureAlways(IsValid(PlayerStateOwnerCharacter)) || !PlayerStateOwnerCharacter->HasAuthority())
	{
		return;
	}

	PlayerStatData.ShieldValue = 0;
}

void UNetherCrownPlayerStatComponent::ModifyMP(float MPDelta)
{
	ANetherCrownPlayerState* OwnerPlayerState{ Cast<ANetherCrownPlayerState>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerPlayerState)))
	{
		return;
	}

	ANetherCrownCharacter* PlayerStateOwnerCharacter{ Cast<ANetherCrownCharacter>(OwnerPlayerState->GetPawn()) };
	if (!ensureAlways(IsValid(PlayerStateOwnerCharacter)) || !PlayerStateOwnerCharacter->HasAuthority())
	{
		return;
	}

	PlayerStatData.CharacterMP += MPDelta;
}
