// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownGameplayTags.h"

namespace NetherCrownTags
{
#pragma region Sound
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_Character, "Sound.Character", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_Character_Knight_JumpStart, "Sound.Character.Knight.JumpStart", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_Character_Knight_HardLand, "Sound.Character.Knight.HardLand", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_Character_Knight_EquipWeapon, "Sound.Character.Knight.EquipWeapon", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_Character_Knight_BasicAttackGrunt, "Sound.Character.Knight.BasicAttackGrunt", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_Character_Knight_SwingWeapon, "Sound.Character.Knight.SwingWeapon", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_Enemy_HurtGrunt, "Sound.Enemy.HurtGrunt", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_Enemy_HurtImpact, "Sound.Enemy.HurtImpact", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_Shield_On, "Sound.Shield.On", "");
#pragma endregion

#pragma region Effect
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Knight, "Effect.Knight", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Knight_BasicAttackImpact, "Effect.Knight.BasicAttackImpact", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_SkyFallSlash_Impact, "Effect.SkyFallSlash.Impact", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_ShieldMastery, "Effect.ShieldMastery", "");
#pragma endregion

#pragma region Weapon
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon, "Weapon", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_Weapon1, "Weapon.Weapon1", "");
#pragma endregion

}
