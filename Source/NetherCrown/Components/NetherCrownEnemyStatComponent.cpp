// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyStatComponent.h"

#include "NetherCrown/Enemy/NetherCrownEnemy.h"

UNetherCrownEnemyStatComponent::UNetherCrownEnemyStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UNetherCrownEnemyStatComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheOwnerEnemy();

	LoadEnemyStatData();
}

void UNetherCrownEnemyStatComponent::CacheOwnerEnemy()
{
	CachedOwnerEnemy = Cast<ANetherCrownEnemy>(GetOwner());
}

void UNetherCrownEnemyStatComponent::LoadEnemyStatData()
{
	if (EnemyStatDataAssetSoft.IsNull())
	{
		return;
	}

	const UNetherCrownEnemyStatData* PlayerStatDataAsset{ EnemyStatDataAssetSoft.LoadSynchronous() };
	if (!ensureAlways(IsValid(PlayerStatDataAsset)))
	{
		return;
	}

	EnemyStatData = PlayerStatDataAsset->GetEnemyStatData();
}

void UNetherCrownEnemyStatComponent::ModifyEnemyHp(float HpDelta)
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	EnemyStatData.EnemyHP += HpDelta;
}
