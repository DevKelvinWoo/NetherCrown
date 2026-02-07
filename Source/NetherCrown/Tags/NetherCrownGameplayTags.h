// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace NetherCrownTags
{
#pragma region Sound
	NETHERCROWN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Character);
	NETHERCROWN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Character_Knight_JumpStart);
	NETHERCROWN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Character_Knight_HardLand);
	NETHERCROWN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Character_Knight_EquipWeapon);
	NETHERCROWN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Character_Knight_BasicAttackGrunt);
	NETHERCROWN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Character_Knight_SwingWeapon);
	NETHERCROWN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Enemy_HurtGrunt);
	NETHERCROWN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Enemy_HurtImpact);
	NETHERCROWN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Shield_On);
#pragma endregion

#pragma region Effect
	NETHERCROWN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Knight);
	NETHERCROWN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Knight_BasicAttackImpact);
	NETHERCROWN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_SkyFallSlash_Impact);
	NETHERCROWN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_ShieldMastery);
	NETHERCROWN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_DashAttack_Impact);
#pragma endregion

#pragma region Weapon
	NETHERCROWN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapon);
	NETHERCROWN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapon_Weapon1);
#pragma endregion
}
