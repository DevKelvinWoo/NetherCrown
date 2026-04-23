// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyActionControlComponent.h"

#include "NetherCrownEnemyBasicAttackComponent.h"
#include "NetherCrown/Components/NetherCrownEnemyDamageReceiverComponent.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"

UNetherCrownEnemyActionControlComponent::UNetherCrownEnemyActionControlComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UNetherCrownEnemyActionControlComponent::CanAttack() const
{
	const UNetherCrownEnemyDamageReceiverComponent* EnemyDamageReceiverComponent{ EnemyDamageReceiverComponentWeak.Get() };
	if (!ensureAlways(IsValid(EnemyDamageReceiverComponent)))
	{
		return false;
	}

	if (EnemyDamageReceiverComponent->IsHitReacting())
	{
		return false;
	}

	return true;
}

bool UNetherCrownEnemyActionControlComponent::CanChase() const
{
	const UNetherCrownEnemyDamageReceiverComponent* EnemyDamageReceiverComponent{ EnemyDamageReceiverComponentWeak.Get() };
	const UNetherCrownEnemyBasicAttackComponent* EnemyBasicAttackComponent{ EnemyBasicAttackComponentWeak.Get() };
	if (!ensureAlways(IsValid(EnemyDamageReceiverComponent)) || !ensureAlways(IsValid(EnemyBasicAttackComponent)))
	{
		return false;
	}

	if (EnemyDamageReceiverComponent->IsHitReacting() || EnemyBasicAttackComponent->IsAttacking())
	{
		return false;
	}

	return true;
}

void UNetherCrownEnemyActionControlComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheInitData();
}

void UNetherCrownEnemyActionControlComponent::CacheInitData()
{
	CachedOwnerEnemy = Cast<ANetherCrownEnemy>(GetOwner());
	if (!ensureAlways(IsValid(CachedOwnerEnemy)))
	{
		return;
	}

	EnemyBasicAttackComponentWeak = CachedOwnerEnemy->GetEnemyBasicAttackComponent();
	EnemyDamageReceiverComponentWeak = CachedOwnerEnemy->GetEnemyDamageReceiverComponent();
}
