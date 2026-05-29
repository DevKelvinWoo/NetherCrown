// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyCrownPrison.h"

#include "Components/CapsuleComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownCrowdControlComponent.h"
#include "NetherCrown/Enemy/NetherCrownBossEnemy.h"
#include "NetherCrown/Enemy/EnemySkill/EnemySkillActor/NetherCrownEnemyCrownPrisonWall.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

void UNetherCrownEnemyCrownPrison::InitEnemySkillObject()
{
	Super::InitEnemySkillObject();

	CacheEnemyCrownPrisonData();
}

void UNetherCrownEnemyCrownPrison::PlayEnemySkillCosmetics()
{
	Super::PlayEnemySkillCosmetics();
}

void UNetherCrownEnemyCrownPrison::ExecuteEnemySkillGameplay()
{
	Super::ExecuteEnemySkillGameplay();

	SpawnCrownPrisonWall();
}

void UNetherCrownEnemyCrownPrison::CacheEnemyCrownPrisonData()
{
	const UNetherCrownEnemySkillDataAsset* EnemySkillDataAsset{ FNetherCrownUtilManager::GetEnemySkillDataAssetByGameplayTag(NetherCrownTags::Enemy_Skill_CrownPrison) };
	const UNetherCrownEnemyCrownPrisonDataAsset* EnemyCrownPrisonDataAsset{ Cast<UNetherCrownEnemyCrownPrisonDataAsset>(EnemySkillDataAsset) };
	if (!ensureAlways(IsValid(EnemyCrownPrisonDataAsset)))
	{
		return;
	}

	CachedCrownEnemyCrownPrisonData = EnemyCrownPrisonDataAsset->GetEnemyCrownPrisonData();
}

void UNetherCrownEnemyCrownPrison::SpawnCrownPrisonWall()
{
	ANetherCrownEnemy* OwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(OwnerEnemy)) || !OwnerEnemy->HasAuthority())
	{
		return;
	}

	UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	FActorSpawnParameters SpawnParameters{};
	SpawnParameters.Owner = OwnerEnemy;
	SpawnParameters.Instigator = OwnerEnemy;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FVector SpawnLocation{ GetCrownPrisonWallSpawnLocation() };
	if (SpawnLocation.IsZero())
	{
		return;
	}

	const FRotator OwnerEnemyRotation{ OwnerEnemy->GetActorRotation() };
	const FRotator SpawnRotation{ 0.f, OwnerEnemyRotation.Yaw, 0.f };

	ANetherCrownEnemyCrownPrisonWall* CrownPrisonWall{ World->SpawnActor<ANetherCrownEnemyCrownPrisonWall>(
		CachedCrownEnemyCrownPrisonData.CrownPrisonWallClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParameters
	) };

	if (!ensureAlways(IsValid(CrownPrisonWall)))
	{
		return;
	}

	FNetherCrownCrownPrisonInitData CrownPrisonInitData{};
	CrownPrisonInitData.ExplosionRadius = CachedCrownEnemyCrownPrisonData.ExplosionRadius;
	CrownPrisonInitData.ExplosionTimeOffset = CachedCrownEnemyCrownPrisonData.ExplosionTimeOffset;
	CrownPrisonInitData.CrownPrisonTagData = CachedCrownEnemyCrownPrisonData.CrownPrisonTagData;

	CrownPrisonWall->GetOnCrownPrisonExplosionHit().AddUObject(this, &ThisClass::HandleOnCrownPrisonExplosionHit);
	CrownPrisonWall->InitCrownPrisonWall(CrownPrisonInitData);
	CrownPrisonWall->StartRiseUpOrDownWall(CachedCrownEnemyCrownPrisonData.WallHiddenZOffset, CachedCrownEnemyCrownPrisonData.WallRiseDuration, true);
}

FVector UNetherCrownEnemyCrownPrison::GetCrownPrisonWallSpawnLocation() const
{
	const ANetherCrownEnemy* OwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(OwnerEnemy)) || !OwnerEnemy->HasAuthority())
	{
		return FVector::ZeroVector;
	}

	const ANetherCrownCharacter* CurrentTargetCharacter{ OwnerEnemy->GetCurrentTargetCharacter() };
	if (!IsValid(CurrentTargetCharacter))
	{
		return FVector::ZeroVector;
	}

	const UCapsuleComponent* TargetCapsuleComponent{ CurrentTargetCharacter->GetCapsuleComponent() };
	const float TargetCapsuleHalfHeight{ IsValid(TargetCapsuleComponent) ? TargetCapsuleComponent->GetScaledCapsuleHalfHeight() : 0.f };
	const FVector GroundLocation{ CurrentTargetCharacter->GetActorLocation() + FVector(0.f, 0.f, CachedCrownEnemyCrownPrisonData.WallHiddenZOffset - TargetCapsuleHalfHeight) };
	return GroundLocation;
}

void UNetherCrownEnemyCrownPrison::HandleOnCrownPrisonExplosionHit(ANetherCrownCharacter* HitCharacter)
{
	const ANetherCrownEnemy* OwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(OwnerEnemy)) || !OwnerEnemy->HasAuthority())
	{
		return;
	}

	if (!IsValid(HitCharacter))
	{
		return;
	}

	ApplyEnemyMagicSkillDamage(HitCharacter);

	UNetherCrownCrowdControlComponent* CCComponent{ HitCharacter->GetCrowdControlComponent() };
	if (!ensureAlways(IsValid(CCComponent)))
	{
		return;
	}

	CCComponent->ApplyCrowdControl(ENetherCrownCrowdControlType::STUN, CachedCrownEnemyCrownPrisonData.CrownPrisonStunDuration);
	CCComponent->Stun();
}
