// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyBTCosmeticComponent.h"

#include "NetherCrown/Data/NetherCrownEnemyBTCosmeticData.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

UNetherCrownEnemyBTCosmeticComponent::UNetherCrownEnemyBTCosmeticComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UNetherCrownEnemyBTCosmeticComponent::PlayEnemyFoundCosmetic()
{
	const ANetherCrownEnemy* OwnerEnemy{ Cast<ANetherCrownEnemy>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerEnemy)) || !OwnerEnemy->HasAuthority())
	{
		return;
	}

	Multicast_PlayEnemyFoundCosmetic();
}

void UNetherCrownEnemyBTCosmeticComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() != NM_DedicatedServer)
	{
		CacheOwnerAnimInstance();
		CacheEnemyBTCosmeticData();
	}
}

void UNetherCrownEnemyBTCosmeticComponent::CacheEnemyBTCosmeticData()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (EnemyBTCosmeticDataSoft.IsNull())
	{
		return;
	}

	CachedEnemyBTCosmeticData = EnemyBTCosmeticDataSoft.LoadSynchronous();

	if (!ensureAlways(IsValid(CachedEnemyBTCosmeticData)))
	{
		return;
	}

	const FNetherCrownEnemyBTFoundCosmeticData& BTFoundCosmeticData{ CachedEnemyBTCosmeticData->GetEnemyFoundCosmeticData() };
	if (!BTFoundCosmeticData.FoundAnimMontageSoft.IsNull())
	{
		CachedEnemyFoundAnimation = BTFoundCosmeticData.FoundAnimMontageSoft.LoadSynchronous();
	}
}

void UNetherCrownEnemyBTCosmeticComponent::CacheOwnerAnimInstance()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	const ANetherCrownEnemy* OwnerEnemy{ Cast<ANetherCrownEnemy>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerEnemy)))
	{
		return;
	}

	const USkeletalMeshComponent* OwnerEnemySkeletalMeshComponent{ OwnerEnemy->GetMesh() };
	if (!ensureAlways(IsValid(OwnerEnemySkeletalMeshComponent)))
	{
		return;
	}

	CachedOwnerAnimInstance = OwnerEnemySkeletalMeshComponent->GetAnimInstance();
}

void UNetherCrownEnemyBTCosmeticComponent::Multicast_PlayEnemyFoundCosmetic_Implementation()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (!ensureAlways(IsValid(CachedEnemyFoundAnimation)) || !ensureAlways(IsValid(CachedOwnerAnimInstance)))
	{
		return;
	}

	CachedOwnerAnimInstance->Montage_Play(CachedEnemyFoundAnimation);

	const FNetherCrownEnemyBTFoundCosmeticData& BTFoundCosmeticData{ CachedEnemyBTCosmeticData->GetEnemyFoundCosmeticData() };
	FNetherCrownUtilManager::PlaySound2DByGameplayTag(this, BTFoundCosmeticData.FoundCosmeticTagData.FoundGruntSoundTag);
}
