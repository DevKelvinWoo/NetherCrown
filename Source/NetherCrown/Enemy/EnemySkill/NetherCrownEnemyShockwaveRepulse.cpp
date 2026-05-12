// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownEnemyShockwaveRepulse.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownCrowdControlComponent.h"
#include "NetherCrown/DamageTypes/NetherCrownDamageEvent.h"
#include "NetherCrown/DamageTypes/NetherCrownMagicDamageType.h"
#include "NetherCrown/Enemy/NetherCrownBossEnemy.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

void UNetherCrownEnemyShockwaveRepulse::InitEnemySkillObject()
{
	Super::InitEnemySkillObject();

	CacheEnemyShockwaveRepulseData();
}

void UNetherCrownEnemyShockwaveRepulse::PlayEnemySkillCosmetics()
{
	Super::PlayEnemySkillCosmetics();
}

void UNetherCrownEnemyShockwaveRepulse::ExecuteEnemySkillGameplay()
{
	Super::ExecuteEnemySkillGameplay();

	StartShockwaveRepulseKnockbackTimer();
}

void UNetherCrownEnemyShockwaveRepulse::CacheEnemyShockwaveRepulseData()
{
	UNetherCrownEnemyShockwaveRepulseDataAsset* LoadedEnemyShockwaveRepulseDataAsset{ Cast<UNetherCrownEnemyShockwaveRepulseDataAsset>(FNetherCrownUtilManager::GetEnemySkillDataAssetByGameplayTag(NetherCrownTags::Enemy_Skill_ShockwaveRepulse)) };
	if (!ensureAlways(IsValid(LoadedEnemyShockwaveRepulseDataAsset)))
	{
		return;
	}

	CachedEnemyShockwaveRepulseData = LoadedEnemyShockwaveRepulseDataAsset->GetEnemyShockwaveRepulseData();
}

void UNetherCrownEnemyShockwaveRepulse::StartShockwaveRepulseKnockbackTimer()
{
	const ANetherCrownEnemy* OwnerBossEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(OwnerBossEnemy)) || !OwnerBossEnemy->HasAuthority())
	{
		return;
	}

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	FTimerManager& TimerManager{ World->GetTimerManager() };
	TimerManager.ClearTimer(ShockwaveRepulseKnockbackTimerHandle);
	TimerManager.SetTimer(ShockwaveRepulseKnockbackTimerHandle, this, &ThisClass::ApplyShockwaveRepulse, CachedEnemyShockwaveRepulseData.ShockwaveRepulseKnockbackTimeOffset, false);
}

void UNetherCrownEnemyShockwaveRepulse::ApplyShockwaveRepulse()
{
	ANetherCrownEnemy* OwnerBossEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(OwnerBossEnemy)) || !OwnerBossEnemy->HasAuthority())
	{
		return;
	}

	ANetherCrownCharacter* CurrentTargetCharacter{ OwnerBossEnemy->GetCurrentTargetCharacter() };
	if (!IsValid(CurrentTargetCharacter))
	{
		return;
	}

	UNetherCrownCrowdControlComponent* CharacterCCComponent{ CurrentTargetCharacter->GetCrowdControlComponent() };
	if (!ensureAlways(IsValid(CharacterCCComponent)))
	{
		return;
	}

	CharacterCCComponent->ApplyCrowdControl(ENetherCrownCrowdControlType::KNOCK_BACK, CachedEnemyShockwaveRepulseData.ShockwaveRepulseDuration);

	const FVector& KnockBackVector{ -CurrentTargetCharacter->GetActorForwardVector() * CachedEnemyShockwaveRepulseData.ShockwaveRepulseKnockbackDistance };
	CharacterCCComponent->KnockBack(KnockBackVector);

	ApplyEnemyMagicSkillDamage(CurrentTargetCharacter);
}
