// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownEnemyTeleport.h"

#include "NavigationSystem.h"
#include "NetherCrown/NetherCrown.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

void UNetherCrownEnemyTeleport::InitEnemySkillObject()
{
	Super::InitEnemySkillObject();

	CacheEnemyTeleportData();
}

void UNetherCrownEnemyTeleport::PlayEnemySkillCosmetics()
{
	Super::PlayEnemySkillCosmetics();

	PlayTeleportSound();
	SpawnTeleportEffect();
}

void UNetherCrownEnemyTeleport::ExecuteEnemySkillGameplay()
{
	Super::ExecuteEnemySkillGameplay();

	TeleportToEscape();
}

void UNetherCrownEnemyTeleport::PlayTeleportSound() const
{
	const ANetherCrownEnemy* OwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(OwnerEnemy)) || OwnerEnemy->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		return;
	}

	FNetherCrownUtilManager::PlaySound2DByGameplayTag(this, CachedTeleportData.TeleportTagData.TeleportSoundTag);
}

void UNetherCrownEnemyTeleport::SpawnTeleportEffect() const
{
	const ANetherCrownEnemy* OwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(OwnerEnemy)) || OwnerEnemy->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		return;
	}

	const FTransform& TeleportEffectTransform{ FTransform(FRotator::ZeroRotator, OwnerEnemy->GetActorLocation(), FVector::OneVector) };
	FNetherCrownUtilManager::SpawnNiagaraSystemByGameplayTag(this, CachedTeleportData.TeleportTagData.TeleportEffectTag, TeleportEffectTransform);
}

void UNetherCrownEnemyTeleport::CacheEnemyTeleportData()
{
	const UNetherCrownEnemySkillDataAsset* EnemySkillDataAsset{ FNetherCrownUtilManager::GetEnemySkillDataAssetByGameplayTag(NetherCrownTags::Enemy_Skill_Teleport) };
	const UNetherCrownEnemyTeleportDataAsset* EnemyTeleportDataAsset{ Cast<UNetherCrownEnemyTeleportDataAsset>(EnemySkillDataAsset) };
	if (!ensureAlways(IsValid(EnemyTeleportDataAsset)))
	{
		return;
	}

	CachedTeleportData = EnemyTeleportDataAsset->GetEnemyTeleportData();
}

void UNetherCrownEnemyTeleport::TeleportToEscape()
{
	ANetherCrownEnemy* OwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(OwnerEnemy)) || !OwnerEnemy->HasAuthority())
	{
		return;
	}

	const ANetherCrownCharacter* CurrentTargetCharacter{ OwnerEnemy->GetCurrentTargetCharacter() };
	if (!IsValid(CurrentTargetCharacter))
	{
		return;
	}

	UNavigationSystemV1* NavSystem{ UNavigationSystemV1::GetNavigationSystem(GetWorld()) };
	if (!ensureAlways(IsValid(NavSystem)))
	{
		return;
	}

	const FVector& TeleportTargetLocation{ CurrentTargetCharacter->GetActorLocation() };

	FNavLocation ProjectedLocation{};
	const bool bProjected{ NavSystem->ProjectPointToNavigation(TeleportTargetLocation, ProjectedLocation) };
	if (!bProjected)
	{
		return;
	}

	FNavLocation RandomTeleportLocation{};
	const bool bFoundTeleportLocation{ NavSystem->GetRandomReachablePointInRadius(ProjectedLocation.Location, CachedTeleportData.TeleportRandomRadius, RandomTeleportLocation) };
	if (!bFoundTeleportLocation)
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("Failed to find teleport location"));
		return;
	}

	OwnerEnemy->SetActorLocation(RandomTeleportLocation.Location);
}
