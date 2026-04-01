// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownEnemyDamageReceiverComponent.h"

#include "NetherCrownCrowdControlComponent.h"
#include "Engine/DamageEvents.h"

#include "NetherCrownCrowdControlTypes.h"
#include "NetherCrownEnemyStatComponent.h"
#include "NetherCrownEquipComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/DamageTypes/UNetherCrownCriticalPhysicalDamageType.h"
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
		HandleEnemyDead();
	}
	else
	{
		const bool bIsCriticalDamage{ DamageEvent.DamageTypeClass == UNetherCrownCriticalPhysicalDamageType::StaticClass() };
		Multicast_PlayTakeDamageAnimation(bIsCriticalDamage);
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

	const bool bIsPhysicalDamage{ DamageEvent.DamageTypeClass->IsChildOf(UNetherCrownPhysicalDamageType::StaticClass()) };

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

void UNetherCrownEnemyDamageReceiverComponent::HandleEnemyDead()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	CachedOwnerEnemy->SetIsDead(true);

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	const float DeathTimingOffset{ EnemyDeathCosmeticData.DestroyTimeOffset };
	World->GetTimerManager().SetTimer(HandleDestroyTimerHandle, this, &ThisClass::HandleDeathTimer, DeathTimingOffset, false);
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
	if (!ensureAlways(IsValid(CachedOwnerEnemy)))
	{
		return;
	}

	if (EnemyDamageAndDeathCosmeticDataAssetSoft.IsNull())
	{
		return;
	}

	const UNetherCrownEnemyDamageAndDeathCosmeticDataAsset* EnemyDamageAndDeathCosmeticDataAsset{ EnemyDamageAndDeathCosmeticDataAssetSoft.LoadSynchronous() };
	if (!ensureAlways(IsValid(EnemyDamageAndDeathCosmeticDataAsset)))
	{
		return;
	}

	EnemyDamageCosmeticData = EnemyDamageAndDeathCosmeticDataAsset->GetEnemyDamageCosmeticData();
	EnemyDeathCosmeticData = EnemyDamageAndDeathCosmeticDataAsset->GetEnemyDeathCosmeticData();

	if (CachedOwnerEnemy->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (!(EnemyDamageCosmeticData.TakeDamageAnimMontageSoft.IsNull()))
	{
		CachedTakeDamageAnimMontage = EnemyDamageCosmeticData.TakeDamageAnimMontageSoft.LoadSynchronous();
	}

	if (!(EnemyDamageCosmeticData.TakeCriticalDamageAnimMontageSoft.IsNull()))
	{
		CachedTakeCriticalDamageAnimMontage = EnemyDamageCosmeticData.TakeCriticalDamageAnimMontageSoft.LoadSynchronous();
	}
}

void UNetherCrownEnemyDamageReceiverComponent::HandleDeathTimer()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	CachedOwnerEnemy->Destroy();
}

void UNetherCrownEnemyDamageReceiverComponent::Multicast_PlayTakeDamageAnimation_Implementation(const bool bIsCriticalDamage)
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || CachedOwnerEnemy->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	const UNetherCrownCrowdControlComponent* CCControlComponent{ CachedOwnerEnemy->GetCrowdControlComponent() };
	if (!ensureAlways(IsValid(CCControlComponent)))
	{
		return;
	}

	if (CCControlComponent->GetCrowdControlType() != ENetherCrownCrowdControlType::NONE)
	{
		return;
	}

	if (!ensureAlways(IsValid(CachedTakeDamageAnimMontage)) || !ensureAlways(IsValid(CachedTakeCriticalDamageAnimMontage)))
	{
		return;
	}

	const USkeletalMeshComponent* SkeletalMeshComponent{ CachedOwnerEnemy->GetMesh() };
	UNetherCrownEnemyAnimInstance* EnemyAnimInstance{ SkeletalMeshComponent ? Cast<UNetherCrownEnemyAnimInstance>(SkeletalMeshComponent->GetAnimInstance()) : nullptr };
	if (!ensureAlways(IsValid(EnemyAnimInstance)))
	{
		return;
	}

	//@NOTE : Critical Damage AnimMontage Code
	//UAnimMontage* TakeDamageAnimMontage{ bIsCriticalDamage ? CachedTakeCriticalDamageAnimMontage : CachedTakeDamageAnimMontage };
	EnemyAnimInstance->Montage_Play(CachedTakeDamageAnimMontage);

	if (bIsCriticalDamage)
	{
		return;
	}

	const TMap<int32, FName>& TakeDamageSectionNameMap{ EnemyDamageCosmeticData.TakeDamageSectionNameMap };
	if (TakeDamageSectionNameMap.IsEmpty())
	{
		return;
	}

	const int32 RandomMontageSectionNum{ FMath::RandRange(0, TakeDamageSectionNameMap.Num() - 1) };
	if (TakeDamageSectionNameMap.Contains(RandomMontageSectionNum))
	{
		EnemyAnimInstance->Montage_JumpToSection(*TakeDamageSectionNameMap.Find(RandomMontageSectionNum), CachedTakeDamageAnimMontage);
	}
}

void UNetherCrownEnemyDamageReceiverComponent::Multicast_PlayTakeDamageSound_Implementation()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || CachedOwnerEnemy->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	FNetherCrownUtilManager::PlaySound2DByGameplayTag(this, EnemyDamageCosmeticData.DamageSoundTagData.EnemyHurtGruntSoundTag);
	FNetherCrownUtilManager::PlaySound2DByGameplayTag(this, EnemyDamageCosmeticData.DamageSoundTagData.EnemyHurtImpactSoundTag);
}
