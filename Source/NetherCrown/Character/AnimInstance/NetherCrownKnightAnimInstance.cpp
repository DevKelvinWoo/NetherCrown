// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownKnightAnimInstance.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownEquipComponent.h"
#include "NetherCrown/Components/NetherCrownSkillComponent.h"
#include "NetherCrown/Weapon/NetherCrownWeapon.h"

void UNetherCrownKnightAnimInstance::AnimNotify_HitSkyFallSlashSkill()
{
	OnHitSkyFallSlashSkill.Broadcast();
}

void UNetherCrownKnightAnimInstance::AnimNotify_HitFrozenTempestSkill()
{
	OnHitFrozenTempestSkill.Broadcast();
}

void UNetherCrownKnightAnimInstance::AnimNotify_ActiveWeaponAuraNiagara()
{
	if (!(IsValid(OwningCharacter)) || OwningCharacter->HasAuthority())
	{
		return;
	}

	if (!(IsValid(SkillComponent)))
	{
		return;
	}

	ANetherCrownWeapon* EquippedWeapon{ EquipComponent ? EquipComponent->GetEquippedWeapon() : nullptr };
	if (!(IsValid(EquippedWeapon)))
	{
		return;
	}

	const ENetherCrownSkillKeyEnum SkillKeyEnum{ SkillComponent->GetActiveSkillKeyEnum() };
	EquippedWeapon->Multicast_ActiveWeaponAuraNiagara(true, SkillKeyEnum);
}

void UNetherCrownKnightAnimInstance::AnimNotify_DeactiveWeaponAuraNiagara()
{
	if (!(IsValid(OwningCharacter)) || OwningCharacter->HasAuthority())
	{
		return;
	}

	ANetherCrownWeapon* EquippedWeapon{ EquipComponent ? EquipComponent->GetEquippedWeapon() : nullptr };
	if (!(IsValid(EquippedWeapon)))
	{
		return;
	}

	EquippedWeapon->Multicast_ActiveWeaponAuraNiagara(false, ENetherCrownSkillKeyEnum::None);
}
