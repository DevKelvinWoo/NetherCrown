// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownPlayerState.h"

#include "Net/UnrealNetwork.h"
#include "NetherCrown/Components/NetherCrownPlayerStatComponent.h"

ANetherCrownPlayerState::ANetherCrownPlayerState()
{
	PrimaryActorTick.bCanEverTick = false;

	NetherCrownPlayerStatComponent = CreateDefaultSubobject<UNetherCrownPlayerStatComponent>(TEXT("PlayerStatComponent"));
	NetherCrownPlayerStatComponent->SetIsReplicated(true);
}

void ANetherCrownPlayerState::SetPersistentPlayerId(const FGuid& InPersistentPlayerId)
{
	if (!HasAuthority())
	{
		return;
	}

	PersistentPlayerId = InPersistentPlayerId;
}

void ANetherCrownPlayerState::SetWeaponPersistentData(const FNetherCrownWeaponPersistentData& InPersistentData)
{
	if (!HasAuthority())
	{
		return;
	}

	WeaponPersistentData = InPersistentData;
}

void ANetherCrownPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	ANetherCrownPlayerState* NetherCrownPlayerState{ Cast<ANetherCrownPlayerState>(PlayerState) };
	if (!IsValid(NetherCrownPlayerState))
	{
		return;
	}

	NetherCrownPlayerState->WeaponPersistentData = WeaponPersistentData;
	NetherCrownPlayerState->PersistentPlayerId = PersistentPlayerId;
}

void ANetherCrownPlayerState::OverrideWith(APlayerState* PlayerState)
{
	Super::OverrideWith(PlayerState);

	ANetherCrownPlayerState* NetherCrownPlayerState{ Cast<ANetherCrownPlayerState>(PlayerState) };
	if (!IsValid(NetherCrownPlayerState))
	{
		return;
	}

	WeaponPersistentData = NetherCrownPlayerState->WeaponPersistentData;
	PersistentPlayerId = NetherCrownPlayerState->PersistentPlayerId;
}

void ANetherCrownPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NetherCrownPlayerStatComponent);
	DOREPLIFETIME(ThisClass, PersistentPlayerId);
}
