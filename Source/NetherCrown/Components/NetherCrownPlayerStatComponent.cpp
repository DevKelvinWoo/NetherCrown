// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownPlayerStatComponent.h"

#include "Net/UnrealNetwork.h"

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

void UNetherCrownPlayerStatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UNetherCrownPlayerStatComponent::AddPlayerShield(int32 InShieldValue)
{
	PlayerStatData.ShieldValue += InShieldValue;
}

void UNetherCrownPlayerStatComponent::ClearPlayerShield()
{
	PlayerStatData.ShieldValue = 0;
}
