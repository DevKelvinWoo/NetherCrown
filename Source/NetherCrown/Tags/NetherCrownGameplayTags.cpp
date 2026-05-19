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
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_Character_Knight_HurtGrunt, "Sound.Character.Knight.HurtGrunt", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_Character_Knight_HurtImpact, "Sound.Character.Knight.HurtImpact", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_Enemy_HurtGrunt, "Sound.Enemy.HurtGrunt", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_Enemy_HurtImpact, "Sound.Enemy.HurtImpact", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_Enemy_DeathGrunt, "Sound.Enemy.DeathGrunt", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_BossEnemy1_HurtGrunt, "Sound.BossEnemy1.HurtGrunt", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_BossEnemy1_DeathGrunt, "Sound.BossEnemy1.DeathGrunt", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_Enemy_Found, "Sound.Enemy.Found", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_FrozenEnemy_HurtImpact, "Sound.FrozenEnemy.HurtImpact", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_FrozenEnemy_Death, "Sound.FrozenEnemy.Death", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_Shield_On, "Sound.Shield.On", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_Projectile_BossEnemy1_ProjectileDestroy, "Sound.Projectile.BossEnemy1.ProjectileDestroy", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_BossEnemy1_CrownPrisonExplosion, "Sound.BossEnemy1.CrownPrisonExplosion", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_BossEnemy1_CrownPrisonRanged, "Sound.BossEnemy1.CrownPrisonRanged", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_BossEnemy1_CrownPrisonStatueRise, "Sound.BossEnemy1.CrownPrisonStatueRise", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_BossEnemy1_CrownPrisonHit, "Sound.BossEnemy1.CrownPrisonHit", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_BossEnemy1_Teleport, "Sound.BossEnemy1.Teleport", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_BossEnemy1_MagicSpikeUp, "Sound.BossEnemy1.MagicSpikeUp", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_BossEnemy1_MagicSpike_Range, "Sound.BossEnemy1.MagicSpike_Range", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sound_BossDungeon_DoorSlide, "Sound.BossDungeon.DoorSlide", "");
#pragma endregion

#pragma region Effect
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Knight, "Effect.Knight", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Knight_BasicAttackImpact, "Effect.Knight.BasicAttackImpact", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Knight_FrozenBasicAttackImpact, "Effect.Knight.FrozenBasicAttackImpact", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_SkyFallSlash_Impact, "Effect.SkyFallSlash.Impact", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_SkyFallSlash_TraceImpact, "Effect.SkyFallSlash.TraceImpact", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_ShieldMastery, "Effect.ShieldMastery", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_DashAttack_Impact, "Effect.DashAttack.Impact", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_DashAttack_ThunderImpact, "Effect.DashAttack.ThunderImpact", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_FrozenDeath_Impact, "Effect.FrozenDeath.Impact", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_FrozenDeath_Impact_BackGround, "Effect.FrozenDeath.Impact.BackGround", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Knight_HitImpact, "Effect.Knight.HitImpact", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Boss1_Projectile_HitImpact, "Effect.Boss1.Projectile.HitImpact", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Boss1_VoidPiercer_HitImpact, "Effect.Boss1.VoidPiercer.HitImpact", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Boss1_CrownPrison_RaiseImpact, "Effect.Boss1.CrownPrison.RaiseImpact", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Boss1_CrownPrison_HitImpact, "Effect.Boss1.CrownPrison.HitImpact", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Boss1_Teleport_Impact, "Effect.Boss1.Teleport.Impact", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Boss1_MagicSpike_Range, "Effect.Boss1.MagicSpike.Range", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Boss1_MagicSpike_Impact, "Effect.Boss1.MagicSpike.Impact", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Boss1_MagicSpike_Hit, "Effect.Boss1.MagicSpike.Hit", "");
#pragma endregion

#pragma region Weapon
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon, "Weapon", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_Weapon1, "Weapon.Weapon1", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_Weapon2, "Weapon.Weapon2", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_Weapon3, "Weapon.Weapon3", "");
#pragma endregion

#pragma region Skill
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Skill, "Skill", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Skill_SkyFallSlash, "Skill.SkyFallSlash", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Skill_ShieldMastery, "Skill.ShieldMastery", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Skill_FrozenTempest, "Skill.FrozenTempest", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Skill_DashAttack, "Skill.DashAttack", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Skill_DodgeSkill, "Skill.DodgeSkill", "");
#pragma endregion

#pragma region UI
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Layer, "UI.Layer", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Layer_Game, "UI.Layer.Game", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Layer_GameMenu, "UI.Layer.GameMenu", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Layer_Menu, "UI.Layer.Menu", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Layer_Modal, "UI.Layer.Modal", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Screen, "UI.Screen", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Screen_PrimaryLayout, "UI.Screen.PrimaryLayout", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Screen_HUD, "UI.Screen.HUD", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Screen_NPCDialogue, "UI.Screen.NPCDialogue", "");
#pragma endregion

#pragma region EnemySkill
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Enemy_Skill_DashAttack, "Enemy.Skill.DashAttack", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Enemy_Skill_VoidPiercer, "Enemy.Skill.VoidPiercer", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Enemy_Skill_CrownPrison, "Enemy.Skill.CrownPrison", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Enemy_Skill_Teleport, "Enemy.Skill.Teleport", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Enemy_Skill_ShockwaveRepulse, "Enemy.Skill.ShockwaveRepulse", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Enemy_Skill_MagicSpike, "Enemy.Skill.MagicSpike", "");
#pragma endregion

#pragma region Sequence
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sequence_Boss1_Begin, "Sequence.Boss1.Begin", "");
#pragma endregion

#pragma region Level
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Level_Boss1Dungeon, "Level.Boss1Dungeon", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Level_NormalMonsterDungeon, "Level.NormalMonsterDungeon", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Level_Town, "Level.Town", "");
#pragma endregion
}
