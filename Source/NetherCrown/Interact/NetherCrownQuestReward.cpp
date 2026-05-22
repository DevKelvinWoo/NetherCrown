// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownQuestReward.h"

#include "Components/CapsuleComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Weapon/NetherCrownWeapon.h"

bool UNetherCrownQuestReward::GrantReward(ANetherCrownCharacter* QuestOwner) const
{
	return IsValid(QuestOwner);
}

bool UNetherCrownQuestWeaponReward::GrantReward(ANetherCrownCharacter* QuestOwner) const
{
	Super::GrantReward(QuestOwner);

	if (!ensureAlways(IsValid(QuestOwner)) || !QuestOwner->HasAuthority())
	{
		return false;
	}

	UWorld* World{ QuestOwner->GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return false;
	}

	FVector GroundLocation{ QuestOwner->GetActorLocation() };
	const UCapsuleComponent* TargetCapsuleComponent{ QuestOwner->GetCapsuleComponent() };
	if (IsValid(TargetCapsuleComponent))
	{
		GroundLocation.Z -= TargetCapsuleComponent->GetScaledCapsuleHalfHeight();
	}

	constexpr float WeaponSpawnOffset{ -200.f };
	GroundLocation.Z += WeaponSpawnOffset;

	FActorSpawnParameters SpawnParams{};
	SpawnParams.ObjectFlags = RF_Transient;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	constexpr float XOffset{ 100.f };
	constexpr float YOffset{ -100.f };
	float WeaponSpawnYOffset{ YOffset };
	for (TSubclassOf<ANetherCrownWeapon> WeaponClass : WeaponClassArr)
	{
		ANetherCrownWeapon* RewardWeapon{ World->SpawnActor<ANetherCrownWeapon>(WeaponClass, GroundLocation + FVector(XOffset, WeaponSpawnYOffset, 0.f), FRotator::ZeroRotator, SpawnParams) };
		if (!ensureAlways(IsValid(RewardWeapon)))
		{
			continue;
		}

		RewardWeapon->RiseUpWeapon();

		WeaponSpawnYOffset -= YOffset;
	}

	return true;
}
