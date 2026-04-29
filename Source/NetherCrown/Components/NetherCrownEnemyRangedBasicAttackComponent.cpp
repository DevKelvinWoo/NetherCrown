// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyRangedBasicAttackComponent.h"

#include "NetherCrown/Data/NetherCrownEnemyProjectileDataAsset.h"
#include "NetherCrown/Data/NetherCrownEnemyRangedBasicAttackDataAsset.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Projectile/NetherCrownEnemyMagicProjectile.h"

UNetherCrownEnemyRangedBasicAttackComponent::UNetherCrownEnemyRangedBasicAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void UNetherCrownEnemyRangedBasicAttackComponent::RequestEnemyRangedAttack()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	SpawnRangedBasicAttackProjectile();
}

void UNetherCrownEnemyRangedBasicAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheInitData();
}

void UNetherCrownEnemyRangedBasicAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UNetherCrownEnemyRangedBasicAttackComponent::CacheEnemyRangedBasicAttackData()
{
	if (EnemyRangedBasicAttackDataAssetSoft.IsNull())
	{
		return;
	}

	CachedEnemyRangedBasicAttackDataAsset = EnemyRangedBasicAttackDataAssetSoft.LoadSynchronous();

	TSoftObjectPtr<UNetherCrownEnemyProjectileDataAsset> EnemyProjectileDataAssetSoft = CachedEnemyRangedBasicAttackDataAsset->GetEnemyRangedBasicAttackData().EnemyProjectileDataAssetSoft;
	if (EnemyProjectileDataAssetSoft.IsNull())
	{
		return;
	}

	CachedProjectileData = EnemyProjectileDataAssetSoft.LoadSynchronous();
}

void UNetherCrownEnemyRangedBasicAttackComponent::CacheInitData()
{
	CachedOwnerEnemy = Cast<ANetherCrownEnemy>(GetOwner());

	CacheEnemyRangedBasicAttackData();
}

void UNetherCrownEnemyRangedBasicAttackComponent::SpawnRangedBasicAttackProjectile()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	if (!ensureAlways(IsValid(CachedProjectileData)))
	{
		return;
	}

	UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	FActorSpawnParameters SpawnParams{};
	SpawnParams.Owner = CachedOwnerEnemy;
	SpawnParams.Instigator = CachedOwnerEnemy;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FNetherCrownEnemyProjectileData& EnemyProjectileData{ CachedProjectileData->GetEnemyProjectileData() };
	if (!ensureAlways(IsValid(EnemyProjectileData.EnemyMagicProjectileClass)))
	{
		return;
	}

	const FVector SpawnLocation{ GetProjectileSpawnLocation() };
	const FRotator SpawnRotation{ CachedOwnerEnemy->GetActorForwardVector().Rotation() };
	ANetherCrownEnemyMagicProjectile* EnemyMagicProjectile{ World->SpawnActor<ANetherCrownEnemyMagicProjectile>(
		EnemyProjectileData.EnemyMagicProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	) };

	if (!ensureAlways(IsValid(EnemyMagicProjectile)))
	{
		return;
	}

	EnemyMagicProjectile->InitProjectile(CachedOwnerEnemy->GetActorForwardVector(), EnemyProjectileData.ProjectileSpeed);
}

FVector UNetherCrownEnemyRangedBasicAttackComponent::GetProjectileSpawnLocation() const
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return FVector::ZeroVector;
	}

	if (!ensureAlways(IsValid(CachedEnemyRangedBasicAttackDataAsset)))
	{
		return FVector::ZeroVector;
	}

	const USkeletalMeshComponent* EnemySkeletalMeshComponent{ CachedOwnerEnemy->GetMesh() };
	if (!ensureAlways(IsValid(EnemySkeletalMeshComponent)))
	{
		return FVector::ZeroVector;
	}

	const FNetherCrownEnemyRangedBasicAttackData& RangedBasicAttackData{ CachedEnemyRangedBasicAttackDataAsset->GetEnemyRangedBasicAttackData() };
	return EnemySkeletalMeshComponent->GetSocketLocation(RangedBasicAttackData.FireSocketName);
}
