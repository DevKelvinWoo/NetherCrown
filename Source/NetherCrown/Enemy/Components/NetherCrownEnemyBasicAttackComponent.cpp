// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyBasicAttackComponent.h"

#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Enemy/AnimInstance/NetherCrownEnemyAnimInstance.h"

UNetherCrownEnemyBasicAttackComponent::UNetherCrownEnemyBasicAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UNetherCrownEnemyBasicAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedOwnerEnemy = Cast<ANetherCrownEnemy>(GetOwner());
	CacheBasicAttackAnimMontage();
}

void UNetherCrownEnemyBasicAttackComponent::CacheBasicAttackAnimMontage()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	if (BasicAttackMontageSoft.IsNull())
	{
		return;
	}

	CachedBasicAttackMontage = BasicAttackMontageSoft.LoadSynchronous();
}

void UNetherCrownEnemyBasicAttackComponent::TestFunc()
{
	bCanAttack = true;
}

void UNetherCrownEnemyBasicAttackComponent::RequestEnemyAttack()
{
	if (bCanAttack)
	{
		Multicast_PlayBasicAttackMontage();

		bCanAttack = false;
		GetWorld()->GetTimerManager().SetTimer(TestTimerHandle, this, &ThisClass::TestFunc, 1.f, false);
	}
}

void UNetherCrownEnemyBasicAttackComponent::Multicast_PlayBasicAttackMontage_Implementation()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	const USkeletalMeshComponent* SkeletalMeshComponent{ CachedOwnerEnemy->GetMesh() };
	if (!ensureAlways(IsValid(SkeletalMeshComponent)))
	{
		return;
	}

	UNetherCrownEnemyAnimInstance* EnemyAnimInstance{ Cast<UNetherCrownEnemyAnimInstance>(SkeletalMeshComponent->GetAnimInstance()) };
	if (!ensureAlways(IsValid(EnemyAnimInstance)))
	{
		return;
	}

	if (!ensureAlways(IsValid(CachedBasicAttackMontage)))
	{
		return;
	}

	EnemyAnimInstance->Montage_Play(CachedBasicAttackMontage);
}
