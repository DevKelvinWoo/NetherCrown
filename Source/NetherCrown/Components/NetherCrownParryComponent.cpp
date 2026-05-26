// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownParryComponent.h"

#include "NetherCrownCrowdControlComponent.h"
#include "NetherCrownPlayerStatComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/DamageTypes/NetherCrownDamageType.h"
#include "NetherCrown/DamageTypes/NetherCrownPhysicalDamageType.h"
#include "NetherCrown/Data/NetherCrownParryData.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/PlayerState/NetherCrownPlayerState.h"
#include "NetherCrown/Projectile/NetherCrownEnemyMagicProjectile.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

UNetherCrownParryComponent::UNetherCrownParryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UNetherCrownParryComponent::ActiveParry()
{
	if (!IsValid(CachedCharacter))
	{
		CacheCharacter();
	}

	SetLocalParryState(true);

	if (IsValid(CachedCharacter) && CachedCharacter->HasAuthority())
	{
		SetParryState(true);
		return;
	}

	Server_ActiveParry();
}

void UNetherCrownParryComponent::DeactivateParry()
{
	if (!IsValid(CachedCharacter))
	{
		CacheCharacter();
	}

	SetLocalParryState(false);

	if (IsValid(CachedCharacter) && CachedCharacter->HasAuthority())
	{
		SetParryState(false);
		return;
	}

	Server_DeactivateParry();
}

bool UNetherCrownParryComponent::IsParrying() const
{
	if (!IsValid(CachedCharacter))
	{
		return false;
	}

	//@NOTE : Locally controlled autonomous proxies should not wait for PlayerState replication to update AnimBP state.
	if (!CachedCharacter->HasAuthority() && CachedCharacter->IsLocallyControlled())
	{
		return bLocalParrying;
	}

	const ANetherCrownPlayerState* PlayerState{ Cast<ANetherCrownPlayerState>(CachedCharacter->GetPlayerState()) };
	if (!ensureAlways(IsValid(PlayerState)))
	{
		return false;
	}

	return PlayerState->IsParrying();
}

void UNetherCrownParryComponent::Parry(AActor* DamageCauser, const TSubclassOf<UDamageType> DamageTypeClass)
{
	if (!IsValid(CachedCharacter) || !CachedCharacter->HasAuthority())
	{
		return;
	}

	ANetherCrownEnemyMagicProjectile* EnemyMagicProjectile{ Cast<ANetherCrownEnemyMagicProjectile>(DamageCauser) };
	if (IsValid(EnemyMagicProjectile))
	{
		Multicast_PlayParrySoundAndEffect();
		ReflectEnemyMagicProjectile(EnemyMagicProjectile);
		return;
	}

	ANetherCrownEnemy* DamageCauserEnemy{ Cast<ANetherCrownEnemy>(DamageCauser) };
	if (IsValid(DamageCauserEnemy) && DamageTypeClass == UNetherCrownPhysicalDamageType::StaticClass())
	{
		Multicast_PlayParrySoundAndEffect();

		UNetherCrownCrowdControlComponent* CCControlComponent{ DamageCauserEnemy->GetCrowdControlComponent() };
		if (!ensureAlways(IsValid(CCControlComponent)))
		{
			return;
		}

		if (!ensureAlways(IsValid(CachedParryDataAsset)))
		{
			return;
		}

		const FNetherCrownParryData& ParryData{ CachedParryDataAsset->GetParryData() };
		CCControlComponent->ApplyCrowdControl(ENetherCrownCrowdControlType::STUN, ParryData.ParryStunDuration);
		CCControlComponent->Stun();
	}
}

void UNetherCrownParryComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheCharacter();
	CacheParryData();
}

void UNetherCrownParryComponent::CacheCharacter()
{
	CachedCharacter = Cast<ANetherCrownCharacter>(GetOwner());
}

void UNetherCrownParryComponent::CacheParryData()
{
	if (ParryDataAssetSoft.IsNull())
	{
		return;
	}

	CachedParryDataAsset = ParryDataAssetSoft.LoadSynchronous();
}

void UNetherCrownParryComponent::ReflectEnemyMagicProjectile(ANetherCrownEnemyMagicProjectile* EnemyMagicProjectile) const
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->HasAuthority())
	{
		return;
	}

	if (!ensureAlways(IsValid(EnemyMagicProjectile)))
	{
		return;
	}

	UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	const FVector ReflectDirection{ GetMagicProjectileReflectDirection(EnemyMagicProjectile) };
	const FVector SpawnLocation{ EnemyMagicProjectile->GetActorLocation() + ReflectDirection * 100.f };
	const FRotator SpawnRotation{ ReflectDirection.Rotation() };

	FActorSpawnParameters SpawnParams{};
	SpawnParams.Owner = CachedCharacter;
	SpawnParams.Instigator = CachedCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ANetherCrownEnemyMagicProjectile* ReflectedProjectile{ World->SpawnActor<ANetherCrownEnemyMagicProjectile>(
		EnemyMagicProjectile->GetClass(),
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	) };

	if (!ensureAlways(IsValid(ReflectedProjectile)))
	{
		EnemyMagicProjectile->Destroy();
		return;
	}

	FNetherCrownEnemyMagicProjectileInitData InitData{};
	InitData.Direction = ReflectDirection;
	InitData.ProjectileSpeed = EnemyMagicProjectile->GetProjectileSpeed();
	InitData.DestroyNiagaraEffectTag = EnemyMagicProjectile->GetDestroyNiagaraEffectTag();
	InitData.DestroySoundTag = EnemyMagicProjectile->GetDestroySoundTag();
	InitData.HitImpactEffectTag = EnemyMagicProjectile->GetHitImpactEffectTag();
	InitData.HitImpactSoundTag = EnemyMagicProjectile->GetHitImpactSoundTag();
	InitData.ProjectileDamage = EnemyMagicProjectile->GetProjectileDamage();
	InitData.OwnerEnemyWeak = MakeWeakObjectPtr(EnemyMagicProjectile->GetOwnerEnemy());
	InitData.ReflectedOwnerCharacterWeak = MakeWeakObjectPtr(CachedCharacter);
	InitData.bReflected = true;

	ReflectedProjectile->InitProjectile(InitData);
	EnemyMagicProjectile->Destroy();
}

FVector UNetherCrownParryComponent::GetMagicProjectileReflectDirection(const ANetherCrownEnemyMagicProjectile* EnemyMagicProjectile) const
{
	if (!ensureAlways(IsValid(CachedCharacter)))
	{
		return FVector::ForwardVector;
	}

	if (!ensureAlways(IsValid(EnemyMagicProjectile)))
	{
		return CachedCharacter->GetActorForwardVector();
	}

	const ANetherCrownEnemy* OwnerEnemy{ EnemyMagicProjectile->GetOwnerEnemy() };
	if (!IsValid(OwnerEnemy))
	{
		return CachedCharacter->GetActorForwardVector();
	}

	const FVector ReflectDirection{ (OwnerEnemy->GetActorLocation() - EnemyMagicProjectile->GetActorLocation()).GetSafeNormal() };
	if (ReflectDirection.IsNearlyZero())
	{
		return CachedCharacter->GetActorForwardVector();
	}

	return ReflectDirection;
}

void UNetherCrownParryComponent::SetLocalParryState(bool bActive)
{
	if (!ensureAlways(IsValid(CachedCharacter)))
	{
		return;
	}

	if (CachedCharacter->HasAuthority() || CachedCharacter->IsLocallyControlled())
	{
		bLocalParrying = bActive;
	}
}

void UNetherCrownParryComponent::SetParryState(bool bActive)
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->HasAuthority())
	{
		return;
	}

	UNetherCrownPlayerStatComponent* PlayerStatComponent{ GetPlayerStatComponent() };
	if (!ensureAlways(IsValid(PlayerStatComponent)))
	{
		return;
	}

	PlayerStatComponent->SetParryingState(bActive);
}

UNetherCrownPlayerStatComponent* UNetherCrownParryComponent::GetPlayerStatComponent() const
{
	if (!ensureAlways(IsValid(CachedCharacter)))
	{
		return nullptr;
	}

	const ANetherCrownPlayerState* PlayerState{ Cast<ANetherCrownPlayerState>(CachedCharacter->GetPlayerState()) };
	if (!ensureAlways(IsValid(PlayerState)))
	{
		return nullptr;
	}

	UNetherCrownPlayerStatComponent* PlayerStatComponent{ PlayerState->GetNetherCrownPlayerStatComponent() };
	if (!ensureAlways(IsValid(PlayerStatComponent)))
	{
		return nullptr;
	}

	return PlayerStatComponent;
}

void UNetherCrownParryComponent::Multicast_PlayParrySoundAndEffect_Implementation()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (!ensureAlways(IsValid(CachedParryDataAsset)) || !ensureAlways(IsValid(CachedCharacter)))
	{
		return;
	}

	const FNetherCrownParryData& ParryData{ CachedParryDataAsset->GetParryData() };
	FNetherCrownUtilManager::PlaySound2DByGameplayTag(GetWorld(), ParryData.ParryTagData.ParrySoundTag);
	FNetherCrownUtilManager::SpawnNiagaraSystemByGameplayTag(this, ParryData.ParryTagData.ParryEffectTag, CachedCharacter->GetActorTransform());
}

void UNetherCrownParryComponent::Server_DeactivateParry_Implementation()
{
	SetParryState(false);
}

void UNetherCrownParryComponent::Server_ActiveParry_Implementation()
{
	SetParryState(true);
}
