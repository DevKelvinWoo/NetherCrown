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

void UNetherCrownPlayerStatComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheOwnerCharacter();

	LoadPlayerStatData();
}

void UNetherCrownPlayerStatComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerStatData);
}

void UNetherCrownPlayerStatComponent::CacheOwnerCharacter()
{
	const ANetherCrownPlayerState* OwnerPlayerState{ Cast<ANetherCrownPlayerState>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerPlayerState)))
	{
		return;
	}

	CachedOwnerCharacter = Cast<ANetherCrownCharacter>(OwnerPlayerState->GetPawn());
}

void UNetherCrownPlayerStatComponent::LoadPlayerStatData()
{
	if (PlayerStatDataAssetSoft.IsNull())
	{
		return;
	}

	const UNetherCrownPlayerStatData* PlayerStatDataAsset{ PlayerStatDataAssetSoft.LoadSynchronous() };
	if (!ensureAlways(IsValid(PlayerStatDataAsset)))
	{
		return;
	}

	PlayerStatData = PlayerStatDataAsset->GetPlayerStatData();
}

void UNetherCrownPlayerStatComponent::OnRep_PlayerStatData(const FNetherCrownPlayerStat& OldPlayerStatData)
{
	if (PlayerStatData.CharacterMP != OldPlayerStatData.CharacterMP)
	{
		OnCharacterMPModified.Broadcast(PlayerStatData.CharacterMP / PlayerStatData.CharacterMaxMP);
	}
}

void UNetherCrownPlayerStatComponent::AddPlayerShield(int32 InShieldValue)
{
	CacheOwnerCharacter();

	if (!ensureAlways(IsValid(CachedOwnerCharacter)) || !CachedOwnerCharacter->HasAuthority())
	{
		return;
	}

	PlayerStatData.ShieldValue += InShieldValue;
}

void UNetherCrownPlayerStatComponent::ClearPlayerShield()
{
	CacheOwnerCharacter();

	if (!ensureAlways(IsValid(CachedOwnerCharacter)) || !CachedOwnerCharacter->HasAuthority())
	{
		return;
	}

	PlayerStatData.ShieldValue = 0;
}

void UNetherCrownPlayerStatComponent::ModifyMP(float MPDelta)
{
	CacheOwnerCharacter();

	if (!ensureAlways(IsValid(CachedOwnerCharacter)) || !CachedOwnerCharacter->HasAuthority())
	{
		return;
	}

	PlayerStatData.CharacterMP += MPDelta;
}
