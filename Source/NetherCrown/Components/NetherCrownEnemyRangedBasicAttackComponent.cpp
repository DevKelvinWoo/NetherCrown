// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyRangedBasicAttackComponent.h"

#include "Net/UnrealNetwork.h"
#include "NetherCrown/NetherCrown.h"
#include "NetherCrown/Data/NetherCrownEnemyProjectileDataAsset.h"
#include "NetherCrown/Data/NetherCrownEnemyRangedBasicAttackDataAsset.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Enemy/AnimInstance/NetherCrownEnemyAnimInstance.h"
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

	if (!ensureAlways(IsValid(CachedEnemyRangedBasicAttackDataAsset)))
	{
		return;
	}

	CurrentComboIndex = 0;

	const TMap<int32, FNetherCrownEnemyRangedBasicAttackComboData>& EnemyRangedBasicAttackComboDataMap{ CachedEnemyRangedBasicAttackDataAsset->GetEnemyRangedBasicAttackData().EnemyRangedBasicAttackComboDataMap };
	const int32 ComboDataMapCount{ EnemyRangedBasicAttackComboDataMap.Num() };
	if (ComboDataMapCount == 0)
	{
		return;
	}

	MaxComboCount = FMath::RandRange(1, ComboDataMapCount);

	StartRangedBasicAttack();
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

void UNetherCrownEnemyRangedBasicAttackComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentComboIndex);
	DOREPLIFETIME(ThisClass, MaxComboCount);
	DOREPLIFETIME(ThisClass, EnemyRangedBasicAttackState);
}

void UNetherCrownEnemyRangedBasicAttackComponent::CacheEnemyRangedBasicAttackData()
{
	if (EnemyRangedBasicAttackDataAssetSoft.IsNull())
	{
		return;
	}

	CachedEnemyRangedBasicAttackDataAsset = EnemyRangedBasicAttackDataAssetSoft.LoadSynchronous();

	const FNetherCrownEnemyRangedBasicAttackData& EnemyRangedBasicAttackData{ CachedEnemyRangedBasicAttackDataAsset->GetEnemyRangedBasicAttackData() };
	if (EnemyRangedBasicAttackData.EnemyProjectileDataAssetSoft.IsNull())
	{
		return;
	}

	CachedProjectileData = EnemyRangedBasicAttackData.EnemyProjectileDataAssetSoft.LoadSynchronous();

	const FNetherCrownEnemyRangedBasicAttackCosmeticData& EnemyRangedBasicAttackCosmeticData{ EnemyRangedBasicAttackData.EnemyRangedBasicAttackCosmeticData };
	if (EnemyRangedBasicAttackCosmeticData.FireAnimMontageSoft.IsNull())
	{
		return;
	}

	CachedBasicAttackMontage = EnemyRangedBasicAttackCosmeticData.FireAnimMontageSoft.LoadSynchronous();
}

void UNetherCrownEnemyRangedBasicAttackComponent::CacheInitData()
{
	CachedOwnerEnemy = Cast<ANetherCrownEnemy>(GetOwner());

	CacheEnemyRangedBasicAttackData();
}

void UNetherCrownEnemyRangedBasicAttackComponent::SpawnRangedBasicAttackProjectile(const FName& StartFireSocketName)
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

	const FVector SpawnLocation{ GetProjectileSpawnLocation(StartFireSocketName) };
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

FVector UNetherCrownEnemyRangedBasicAttackComponent::GetProjectileSpawnLocation(const FName& StartFireSocketName) const
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

	return EnemySkeletalMeshComponent->GetSocketLocation(StartFireSocketName);
}

void UNetherCrownEnemyRangedBasicAttackComponent::StartRangedBasicAttack()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	if (!ensureAlways(IsValid(CachedEnemyRangedBasicAttackDataAsset)))
	{
		return;
	}

	EnemyRangedBasicAttackState = ENetherCrownEnemyRangedBasicAttackState::Attacking;

	PlayAttackMontageAndSpawnProjectile();

	++CurrentComboIndex;

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	FTimerManager& TimerManager{ World->GetTimerManager() };
	TimerManager.ClearTimer(ComboAttackTimerHandle);

	const TMap<int32, FNetherCrownEnemyRangedBasicAttackComboData>& EnemyRangedBasicAttackComboDataMap{ CachedEnemyRangedBasicAttackDataAsset->GetEnemyRangedBasicAttackData().EnemyRangedBasicAttackComboDataMap };
	const FNetherCrownEnemyRangedBasicAttackComboData* FoundComboDataPtr{ EnemyRangedBasicAttackComboDataMap.Find(CurrentComboIndex - 1) };
	if (!FoundComboDataPtr)
	{
		return;
	}

	if (MaxComboCount == 1)
	{
		TimerManager.SetTimer(ComboAttackEndTimerHandle, this, &ThisClass::EndRangedBasicAttack, FoundComboDataPtr->ComboEndTime, false);
		return;
	}

	if (CurrentComboIndex < MaxComboCount)
	{
		TimerManager.SetTimer(ComboAttackTimerHandle, this, &ThisClass::StartRangedBasicAttack, FoundComboDataPtr->ComboEndTime, false);
	}
	else
	{
		TimerManager.SetTimer(ComboAttackEndTimerHandle, this, &ThisClass::EndRangedBasicAttack, FoundComboDataPtr->ComboEndTime, false);
	}
}

void UNetherCrownEnemyRangedBasicAttackComponent::PlayAttackMontageAndSpawnProjectile()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	if (!ensureAlways(IsValid(CachedEnemyRangedBasicAttackDataAsset)))
	{
		return;
	}

	const TMap<int32, FNetherCrownEnemyRangedBasicAttackComboData>& EnemyRangedBasicAttackComboDataMap{ CachedEnemyRangedBasicAttackDataAsset->GetEnemyRangedBasicAttackData().EnemyRangedBasicAttackComboDataMap };
	const FNetherCrownEnemyRangedBasicAttackComboData* RangedBasicAttackComboDataPtr{ EnemyRangedBasicAttackComboDataMap.Find(CurrentComboIndex) };
	if (!RangedBasicAttackComboDataPtr)
	{
		return;
	}

	Multicast_PlayRangedBasicAttackAnim(RangedBasicAttackComboDataPtr->ComboMontageSectionName);
	SpawnRangedBasicAttackProjectile(RangedBasicAttackComboDataPtr->FireSocketName);
}

void UNetherCrownEnemyRangedBasicAttackComponent::EndRangedBasicAttack()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	FTimerManager& TimerManager{ World->GetTimerManager() };
	TimerManager.ClearTimer(ComboAttackEndTimerHandle);

	EnemyRangedBasicAttackState = ENetherCrownEnemyRangedBasicAttackState::NotAttacking;

	OnEnemyRangedBasicAttackFinished.Broadcast();
}

void UNetherCrownEnemyRangedBasicAttackComponent::Multicast_PlayRangedBasicAttackAnim_Implementation(const FName& ComboSectionName)
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (!ensureAlways(IsValid(CachedBasicAttackMontage)) || !ensureAlways(IsValid(CachedOwnerEnemy)))
	{
		return;
	}

	const USkeletalMeshComponent* EnemySkeletalMeshComponent{ CachedOwnerEnemy->GetMesh() };
	if (!ensureAlways(IsValid(EnemySkeletalMeshComponent)))
	{
		return;
	}

	UNetherCrownEnemyAnimInstance* EnemyAnimInstance{ Cast<UNetherCrownEnemyAnimInstance>(EnemySkeletalMeshComponent->GetAnimInstance()) };
	if (!ensureAlways(IsValid(EnemyAnimInstance)))
	{
		return;
	}

	EnemyAnimInstance->Montage_Play(CachedBasicAttackMontage);
	EnemyAnimInstance->Montage_JumpToSection(ComboSectionName, CachedBasicAttackMontage);
}
