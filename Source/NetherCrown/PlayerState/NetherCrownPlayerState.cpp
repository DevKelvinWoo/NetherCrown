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

void ANetherCrownPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NetherCrownPlayerStatComponent);
}
