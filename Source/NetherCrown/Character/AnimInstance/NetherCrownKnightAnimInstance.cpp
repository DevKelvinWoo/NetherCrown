// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownKnightAnimInstance.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownEquipComponent.h"
#include "NetherCrown/Components/NetherCrownSkillComponent.h"
#include "NetherCrown/Weapon/NetherCrownWeapon.h"

void UNetherCrownKnightAnimInstance::AnimNotify_HitSkyFallSlashSkill()
{
	if (!IsValid(CachedOwningCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedOwningCharacter is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	OnHitSkyFallSlashSkill.Broadcast();
}

void UNetherCrownKnightAnimInstance::AnimNotify_HitFrozenTempestSkill()
{
	if (!IsValid(CachedOwningCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedOwningCharacter is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	OnHitFrozenTempestSkill.Broadcast();
}

void UNetherCrownKnightAnimInstance::AnimNotify_ActiveWeaponAuraNiagara()
{
	if (!(IsValid(CachedOwningCharacter)))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedOwningCharacter is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	if (!(IsValid(CachedSkillComponent)))
	{
		UE_LOG(LogTemp, Warning, TEXT("CachedSkillComponent is not valid %hs"), __FUNCTION__);
		return;
	}

	ANetherCrownWeapon* EquippedWeapon{ CachedEquipComponent ? CachedEquipComponent->GetEquippedWeapon() : nullptr };
	if (!(IsValid(EquippedWeapon)))
	{
		UE_LOG(LogTemp, Warning, TEXT("EquippedWeapon is not valid %hs"), __FUNCTION__);
		return;
	}

	const ENetherCrownSkillKeyEnum SkillKeyEnum{ CachedSkillComponent->GetActiveSkillKeyEnum() };
	EquippedWeapon->Multicast_ActiveWeaponAuraNiagara(true, SkillKeyEnum);
}

void UNetherCrownKnightAnimInstance::AnimNotify_DeactiveWeaponAuraNiagara()
{
	if (!(IsValid(CachedOwningCharacter)))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedOwningCharacter is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	ANetherCrownWeapon* EquippedWeapon{ CachedEquipComponent ? CachedEquipComponent->GetEquippedWeapon() : nullptr };
	if (!(IsValid(EquippedWeapon)))
	{
		UE_LOG(LogTemp, Warning, TEXT("EquippedWeapon is not valid %hs"), __FUNCTION__);
		return;
	}

	EquippedWeapon->Multicast_ActiveWeaponAuraNiagara(false, ENetherCrownSkillKeyEnum::None);
}
