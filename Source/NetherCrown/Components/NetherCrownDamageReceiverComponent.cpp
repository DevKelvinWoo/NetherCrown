// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownDamageReceiverComponent.h"

#include "NetherCrownPlayerStatComponent.h"
#include "Engine/DamageEvents.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/DamageTypes/NetherCrownPhysicalDamageType.h"
#include "NetherCrown/PlayerState/NetherCrownPlayerState.h"

UNetherCrownDamageReceiverComponent::UNetherCrownDamageReceiverComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

float UNetherCrownDamageReceiverComponent::HandleIncomingDamage(float DamageAmount, FDamageEvent const& DamageEvent, AActor* DamageCauser)
{
	const float FinalDamage{ CalculateFinalDamage(DamageAmount, DamageEvent, DamageCauser) };

	ApplyFinalDamage(FinalDamage);

	if (IsDead())
	{
		UE_LOG(LogTemp, Warning, TEXT("Character is dead"));
		//HandleEnemyDead();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Character is alive, just damaged"));
		// const bool bIsCriticalDamage{ DamageEvent.DamageTypeClass == UNetherCrownCriticalPhysicalDamageType::StaticClass() };
		// Multicast_PlayTakeDamageAnimation(bIsCriticalDamage);
		// Multicast_PlayTakeDamageSound();
	}

	return FinalDamage;
}

void UNetherCrownDamageReceiverComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheOwnerCharacter();
}

void UNetherCrownDamageReceiverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UNetherCrownDamageReceiverComponent::CacheOwnerCharacter()
{
	CachedOwnerCharacter = Cast<ANetherCrownCharacter>(GetOwner());
}

float UNetherCrownDamageReceiverComponent::CalculateFinalDamage(float DamageAmount, FDamageEvent const& DamageEvent, const AActor* DamageCauser) const
{
	if (!ensureAlways(IsValid(CachedOwnerCharacter)) || !CachedOwnerCharacter->HasAuthority())
	{
		return 0.f;
	}

	const bool bIsPhysicalDamage{ DamageEvent.DamageTypeClass->IsChildOf(UNetherCrownPhysicalDamageType::StaticClass()) };

	const int32 ArmorStat{ GetArmorStat(bIsPhysicalDamage) };
	const float DamageMultiplier{ 100.f / (100.f + ArmorStat) };
	const int32 FinalDamage{ FMath::RoundToInt(DamageAmount * DamageMultiplier) };

	return FinalDamage;
}

void UNetherCrownDamageReceiverComponent::ApplyFinalDamage(float FinalDamage)
{
	if (!ensureAlways(IsValid(CachedOwnerCharacter)) || !CachedOwnerCharacter->HasAuthority())
	{
		return;
	}

	const ANetherCrownPlayerState* OwnerPlayerState{ Cast<ANetherCrownPlayerState>(CachedOwnerCharacter->GetPlayerState()) };
	if (!ensureAlways(IsValid(OwnerPlayerState)))
	{
		return;
	}

	UNetherCrownPlayerStatComponent* PlayerStatComponent{ OwnerPlayerState->GetNetherCrownPlayerStatComponent() };
	if (!ensureAlways(IsValid(PlayerStatComponent)))
	{
		return;
	}

	PlayerStatComponent->ModifyHp(-FinalDamage);
}

int32 UNetherCrownDamageReceiverComponent::GetArmorStat(const bool bIsPhysicalDamage) const
{
	if (!ensureAlways(IsValid(CachedOwnerCharacter)) || !CachedOwnerCharacter->HasAuthority())
	{
		return 0;
	}

	const ANetherCrownPlayerState* OwnerPlayerState{ Cast<ANetherCrownPlayerState>(CachedOwnerCharacter->GetPlayerState()) };
	if (!ensureAlways(IsValid(OwnerPlayerState)))
	{
		return 0;
	}


	const UNetherCrownPlayerStatComponent* PlayerStatComponent{ OwnerPlayerState->GetNetherCrownPlayerStatComponent() };
	if (!ensureAlways(IsValid(PlayerStatComponent)))
	{
		return 0;
	}

	const FNetherCrownPlayerStat& PlayerStat{ PlayerStatComponent->GetPlayerStatData() };

	return bIsPhysicalDamage ? PlayerStat.PhysicalArmor : PlayerStat.MagicArmor;
}

bool UNetherCrownDamageReceiverComponent::IsDead() const
{
	if (!ensureAlways(IsValid(CachedOwnerCharacter)) || !CachedOwnerCharacter->HasAuthority())
	{
		return false;
	}

	const ANetherCrownPlayerState* OwnerPlayerState{ Cast<ANetherCrownPlayerState>(CachedOwnerCharacter->GetPlayerState()) };
	if (!ensureAlways(IsValid(OwnerPlayerState)))
	{
		return false;
	}


	const UNetherCrownPlayerStatComponent* PlayerStatComponent{ OwnerPlayerState->GetNetherCrownPlayerStatComponent() };
	if (!ensureAlways(IsValid(PlayerStatComponent)))
	{
		return false;
	}

	const FNetherCrownPlayerStat& PlayerStat{ PlayerStatComponent->GetPlayerStatData() };
	const int32 CurrentHP{ PlayerStat.CharacterHP };

	return CurrentHP <= 0;
}
