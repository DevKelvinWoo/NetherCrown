// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownEnemyDamageReceiverComponent.h"

#include "Engine/DamageEvents.h"

#include "NetherCrownCrowdControlTypes.h"
#include "NetherCrownEnemyStatComponent.h"
#include "NetherCrownEquipComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/DamageTypes/NetherCrownPhysicalDamageType.h"
#include "NetherCrown/Data/NetherCrownWeaponData.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Enemy/AnimInstance/NetherCrownEnemyAnimInstance.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

UNetherCrownEnemyDamageReceiverComponent::UNetherCrownEnemyDamageReceiverComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UNetherCrownEnemyDamageReceiverComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedOwnerEnemy = Cast<ANetherCrownEnemy>(GetOwner());

	LoadEnemyDamageCosmeticData();
}

float UNetherCrownEnemyDamageReceiverComponent::HandleIncomingDamage(float DamageAmount, FDamageEvent const& DamageEvent, const AActor* DamageCauser)
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return 0.f;
	}

	const float FinalDamage{ CalculateFinalDamage(DamageAmount, DamageEvent, DamageCauser) };

	ApplyFinalDamage(FinalDamage);

	if (IsDead())
	{
		//@NOTE : Handling Death
	}
	else
	{
		Multicast_PlayTakeDamageAnimation(ENetherCrownCrowdControlType::NONE);
		Multicast_PlayTakeDamageSound();
	}

	return FinalDamage;
}

float UNetherCrownEnemyDamageReceiverComponent::CalculateFinalDamage(float DamageAmount, FDamageEvent const& DamageEvent, const AActor* DamageCauser) const
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return 0.f;
	}

	const bool bIsPhysicalDamage{ DamageEvent.DamageTypeClass == UNetherCrownPhysicalDamageType::StaticClass() };

	const int32 ArmorStat{ GetArmorStat(bIsPhysicalDamage) };
	const int32 EffectiveArmor{ FMath::Max(0, ArmorStat - GetWeaponPenetration(bIsPhysicalDamage, DamageCauser)) };

	const float DamageMultiplier{ 100.f / (100.f + EffectiveArmor) };
	const int32 FinalDamage{ FMath::RoundToInt(DamageAmount * DamageMultiplier) };

	return FinalDamage;
}

void UNetherCrownEnemyDamageReceiverComponent::ApplyFinalDamage(float FinalDamage)
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	UNetherCrownEnemyStatComponent* EnemyStatComponent{ CachedOwnerEnemy->GetEnemyStatComponent() };
	if (!ensureAlways(IsValid(EnemyStatComponent)))
	{
		return;
	}

	const FNetherCrownEnemyStat& EnemyStatData{ EnemyStatComponent->GetEnemyStatData() };
	const int32 CurrentHP = EnemyStatData.EnemyHP;
	const int32 NewHP = FMath::Max(0, CurrentHP - FinalDamage);

	EnemyStatComponent->SetEnemyHp(NewHP);
}

bool UNetherCrownEnemyDamageReceiverComponent::IsDead() const
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return false;
	}

	const UNetherCrownEnemyStatComponent* EnemyStatComponent{ CachedOwnerEnemy->GetEnemyStatComponent() };
	if (!ensureAlways(IsValid(EnemyStatComponent)))
	{
		return false;
	}

	const int32 CurrentHP{ EnemyStatComponent->GetEnemyStatData().EnemyHP };
	return CurrentHP <= 0;
}

int32 UNetherCrownEnemyDamageReceiverComponent::GetWeaponPenetration(const bool bIsPhysicalDamage, const AActor* DamageCauser) const
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return 0;
	}

	const ANetherCrownCharacter* NetherCrownCharacter = Cast<ANetherCrownCharacter>(DamageCauser);
	if (!ensureAlways(IsValid(NetherCrownCharacter)))
	{
		return 0;
	}

	UNetherCrownEquipComponent* EquipComponent{ NetherCrownCharacter->GetEquipComponent() };
	if (!ensureAlways(IsValid(EquipComponent)))
	{
		return 0;
	}

	const UNetherCrownWeaponData* WeaponData{ EquipComponent->GetEquippedWeaponData() };
	if (!ensureAlways(IsValid(WeaponData)))
	{
		return 0;
	}

	return bIsPhysicalDamage ? WeaponData->GetPhysicalPenetration() : WeaponData->GetMagicPenetration();
}

int32 UNetherCrownEnemyDamageReceiverComponent::GetArmorStat(const bool bIsPhysicalDamage) const
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return 0;
	}

	const UNetherCrownEnemyStatComponent* EnemyStatComponent{ CachedOwnerEnemy->GetEnemyStatComponent() };
	if (!ensureAlways(IsValid(EnemyStatComponent)))
	{
		return 0;
	}

	const FNetherCrownEnemyStat& EnemyStat{ EnemyStatComponent->GetEnemyStatData() };

	return bIsPhysicalDamage ? EnemyStat.PhysicalArmor : EnemyStat.MagicArmor;
}

void UNetherCrownEnemyDamageReceiverComponent::LoadEnemyDamageCosmeticData()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	if (EnemyDamageCosmeticDataAssetSoft.IsNull())
	{
		return;
	}

	const UNetherCrownEnemyDamageCosmeticDataAsset* EnemyDamageCosmeticDataAsset{ EnemyDamageCosmeticDataAssetSoft.LoadSynchronous() };
	if (!ensureAlways(IsValid(EnemyDamageCosmeticDataAsset)))
	{
		return;
	}

	EnemyDamageCosmeticData = EnemyDamageCosmeticDataAsset->GetEnemyDamageCosmeticData();

	if (!(EnemyDamageCosmeticData.TakeDamageAnimMontageSoft.IsNull()))
	{
		CachedTakeDamageAnimMontage = EnemyDamageCosmeticData.TakeDamageAnimMontageSoft.LoadSynchronous();
	}
}

void UNetherCrownEnemyDamageReceiverComponent::Multicast_PlayTakeDamageAnimation_Implementation(const ENetherCrownCrowdControlType InCrowdControlType)
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	if (InCrowdControlType != ENetherCrownCrowdControlType::NONE)
	{
		return;
	}

	if (!ensureAlways(IsValid(CachedTakeDamageAnimMontage)))
	{
		return;
	}

	const USkeletalMeshComponent* SkeletalMeshComponent{ CachedOwnerEnemy->GetMesh() };
	UNetherCrownEnemyAnimInstance* EnemyAnimInstance{ SkeletalMeshComponent ? Cast<UNetherCrownEnemyAnimInstance>(SkeletalMeshComponent->GetAnimInstance()) : nullptr };
	if (!ensureAlways(IsValid(EnemyAnimInstance)))
	{
		return;
	}

	EnemyAnimInstance->Montage_Play(CachedTakeDamageAnimMontage);
}

void UNetherCrownEnemyDamageReceiverComponent::Multicast_PlayTakeDamageSound_Implementation()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	FNetherCrownUtilManager::PlaySound2DByGameplayTag(this, EnemyDamageCosmeticData.DamageSoundTagData.EnemyHurtGruntSoundTag);
	FNetherCrownUtilManager::PlaySound2DByGameplayTag(this, EnemyDamageCosmeticData.DamageSoundTagData.EnemyHurtImpactSoundTag);
}
