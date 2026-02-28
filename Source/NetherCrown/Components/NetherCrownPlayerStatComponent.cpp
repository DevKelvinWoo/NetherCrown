// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownPlayerStatComponent.h"

#include "NetherCrown/NetherCrown.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"

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

void UNetherCrownPlayerStatComponent::CacheCharacter()
{
	CachedCharacter = Cast<ANetherCrownCharacter>(GetOwner());
}

void UNetherCrownPlayerStatComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheCharacter();
}

void UNetherCrownPlayerStatComponent::AddPlayerShield(int32 InShieldValue)
{
	if (!IsValid(CachedCharacter))
	{
		UE_LOG(LogNetherCrown, Error, TEXT("CachedCharacter is invalid."));
		return;
	}

	if (!CachedCharacter->HasAuthority())
	{
		return;
	}

	PlayerStatData.ShieldValue += InShieldValue;
}

void UNetherCrownPlayerStatComponent::ClearPlayerShield()
{
	if (!IsValid(CachedCharacter))
	{
		UE_LOG(LogNetherCrown, Error, TEXT("CachedCharacter is invalid."));
		return;
	}

	if (!CachedCharacter->HasAuthority())
	{
		return;
	}

	PlayerStatData.ShieldValue = 0;
}
