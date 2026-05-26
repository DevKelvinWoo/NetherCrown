// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyStatComponent.h"

#include "Net/UnrealNetwork.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"

UNetherCrownEnemyStatComponent::UNetherCrownEnemyStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UNetherCrownEnemyStatComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheOwnerEnemy();

	LoadEnemyStatData();
}

void UNetherCrownEnemyStatComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EnemyStatData);
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

void UNetherCrownEnemyStatComponent::OnRep_EnemyStatData(const FNetherCrownEnemyStat& OldEnemyStatData)
{
	if (EnemyStatData.EnemyHP != OldEnemyStatData.EnemyHP)
	{
		OnEnemyHPModified.Broadcast(GetEnemyHPRatio());
	}
}

float UNetherCrownEnemyStatComponent::GetEnemyHPRatio() const
{
	if (EnemyStatData.EnemyMaxHP <= 0)
	{
		return 0.f;
	}

	return FMath::Clamp(static_cast<float>(EnemyStatData.EnemyHP) / static_cast<float>(EnemyStatData.EnemyMaxHP), 0.f, 1.f);
}

void UNetherCrownEnemyStatComponent::ModifyEnemyHp(float HpDelta)
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	EnemyStatData.EnemyHP += HpDelta;

	OnEnemyHPModified.Broadcast(GetEnemyHPRatio());
}
