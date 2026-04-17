// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownDamageReceiverComponent.h"

#include "NetherCrownCrowdControlComponent.h"
#include "NetherCrownPlayerStatComponent.h"
#include "Engine/DamageEvents.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Character/NetherCrownPlayerController.h"
#include "NetherCrown/Character/AnimInstance/NetherCrownKnightAnimInstance.h"
#include "NetherCrown/DamageTypes/NetherCrownPhysicalDamageType.h"
#include "NetherCrown/Data/NetherCrownCharacterDamageReceiveDataAsset.h"
#include "NetherCrown/PlayerState/NetherCrownPlayerState.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

UNetherCrownDamageReceiverComponent::UNetherCrownDamageReceiverComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

float UNetherCrownDamageReceiverComponent::HandleIncomingDamage(float DamageAmount, FDamageEvent const& DamageEvent, AActor* DamageCauser)
{
	if (!ensureAlways(IsValid(CachedOwnerCharacter)) || !CachedOwnerCharacter->HasAuthority())
	{
		return 0.f;
	}

	const float FinalDamage{ CalculateFinalDamage(DamageAmount, DamageEvent, DamageCauser) };
	ApplyFinalDamage(FinalDamage);

	Multicast_PlayHitImpactEffect();
	Client_PlayHitCameraShake();

	if (IsDead())
	{
		//HandleEnemyDead();
	}
	else
	{
		Multicast_PlayHitImpactSound();
		Multicast_PlayHitReactAnimation();

		// const bool bIsCriticalDamage{ DamageEvent.DamageTypeClass == UNetherCrownCriticalPhysicalDamageType::StaticClass() };
		// Multicast_PlayTakeDamageAnimation(bIsCriticalDamage);
		// Multicast_PlayTakeDamageSound();
	}

	return FinalDamage;
}

void UNetherCrownDamageReceiverComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheOwnerCharacter();
	CacheDamageReceiveDataAsset();
	CacheHitReactAnimMontage();
}

void UNetherCrownDamageReceiverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UNetherCrownDamageReceiverComponent::CacheOwnerCharacter()
{
	CachedOwnerCharacter = Cast<ANetherCrownCharacter>(GetOwner());
}

void UNetherCrownDamageReceiverComponent::CacheDamageReceiveDataAsset()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (!DamageReceiveDataAssetSoft.IsNull())
	{
		CachedDamageReceiveDataAsset = DamageReceiveDataAssetSoft.LoadSynchronous();
	}
}

void UNetherCrownDamageReceiverComponent::CacheHitReactAnimMontage()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (!ensureAlways(IsValid(CachedDamageReceiveDataAsset)))
	{
		return;
	}

	const FNetherCrownCharacterDamageReceiveData& DamageReceiveData{ CachedDamageReceiveDataAsset->GetDamageReceiveData() };
	if (!DamageReceiveData.HitReactAnimMontageSoft.IsNull())
	{
		CachedHitReactAnimMontage = DamageReceiveData.HitReactAnimMontageSoft.LoadSynchronous();
	}
}

void UNetherCrownDamageReceiverComponent::Client_PlayHitCameraShake_Implementation()
{
	if (!ensureAlways(IsValid(CachedOwnerCharacter)))
	{
		return;
	}

	const ANetherCrownPlayerController* PlayerController{ Cast<ANetherCrownPlayerController>(CachedOwnerCharacter->GetController()) };
	if (!ensureAlways(IsValid(PlayerController)))
	{
		return;
	}

	APlayerCameraManager* CameraManager{ PlayerController->PlayerCameraManager };
	if (!ensureAlways(IsValid(CameraManager)))
	{
		return;
	}

	const FNetherCrownCharacterDamageReceiveData& DamageReceiveData{ CachedDamageReceiveDataAsset->GetDamageReceiveData() };
	CameraManager->StartCameraShake(DamageReceiveData.HitCameraShakeClass, 1.0f);
}

void UNetherCrownDamageReceiverComponent::Multicast_PlayHitImpactEffect_Implementation()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	const FNetherCrownCharacterDamageReceiveData& DamageReceiveData{ CachedDamageReceiveDataAsset->GetDamageReceiveData() };
	FNetherCrownUtilManager::SpawnNiagaraSystemByGameplayTag(CachedOwnerCharacter->GetWorld(), DamageReceiveData.DamageReceiveTagData.HitImpactTag, CachedOwnerCharacter->GetActorTransform());
}

void UNetherCrownDamageReceiverComponent::Multicast_PlayHitReactAnimation_Implementation()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (!ensureAlways(IsValid(CachedOwnerCharacter)))
	{
		return;
	}

	const UNetherCrownCrowdControlComponent* OwnerCharacterCCComponent{ CachedOwnerCharacter->GetCrowdControlComponent() };
	if (!ensureAlways(IsValid(OwnerCharacterCCComponent)))
	{
		return;
	}

	if (OwnerCharacterCCComponent->GetCrowdControlType() != ENetherCrownCrowdControlType::NONE)
	{
		return;
	}

	const USkeletalMeshComponent* OwnerSkeletalMeshComponent{ CachedOwnerCharacter->GetMesh() };
	if (!ensureAlways(IsValid(OwnerSkeletalMeshComponent)))
	{
		return;
	}

	UNetherCrownKnightAnimInstance* OwnerKnightAnimInstance{ Cast<UNetherCrownKnightAnimInstance>(OwnerSkeletalMeshComponent->GetAnimInstance()) };
	if (!ensureAlways(IsValid(OwnerKnightAnimInstance)))
	{
		return;
	}

	if (!ensureAlways(IsValid(CachedHitReactAnimMontage)))
	{
		return;
	}

	OwnerKnightAnimInstance->Montage_Play(CachedHitReactAnimMontage);

	if (!ensureAlways(IsValid(CachedDamageReceiveDataAsset)))
	{
		return;
	}

	const FNetherCrownCharacterDamageReceiveData& DamageReceiveData{ CachedDamageReceiveDataAsset->GetDamageReceiveData() };
	const TMap<int32, FName>& HitReactSectionNameMap{ DamageReceiveData.HitReactSectionNameMap };
	const int32 RandomMontageSectionNum{ FMath::RandRange(0, HitReactSectionNameMap.Num() - 1) };
	if (HitReactSectionNameMap.Contains(RandomMontageSectionNum))
	{
		OwnerKnightAnimInstance->Montage_JumpToSection(*HitReactSectionNameMap.Find(RandomMontageSectionNum), CachedHitReactAnimMontage);
	}
}

void UNetherCrownDamageReceiverComponent::Multicast_PlayHitImpactSound_Implementation()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (!ensureAlways(IsValid(CachedOwnerCharacter)))
	{
		return;
	}

	if (!ensureAlways(IsValid(CachedDamageReceiveDataAsset)))
	{
		return;
	}

	const FNetherCrownCharacterDamageReceiveData& DamageReceiveData{ CachedDamageReceiveDataAsset->GetDamageReceiveData() };
	FNetherCrownUtilManager::PlaySound2DByGameplayTag(CachedOwnerCharacter->GetWorld(), DamageReceiveData.DamageReceiveTagData.HitGruntSoundTag);
}

float UNetherCrownDamageReceiverComponent::CalculateFinalDamage(float DamageAmount, FDamageEvent const& DamageEvent, const AActor* DamageCauser) const
{
	if (!ensureAlways(IsValid(CachedOwnerCharacter)) || !CachedOwnerCharacter->HasAuthority())
	{
		return 0.f;
	}

	const bool bIsPhysicalDamage{ DamageEvent.DamageTypeClass->IsChildOf(UNetherCrownPhysicalDamageType::StaticClass()) };

	const int32 ArmorStat{ GetArmorStat(bIsPhysicalDamage) };
	const float DamageMultiplier{ 100.f / (100.f + ArmorStat) };
	const int32 FinalDamage{ FMath::RoundToInt(DamageAmount * DamageMultiplier) };

	return FinalDamage;
}

void UNetherCrownDamageReceiverComponent::ApplyFinalDamage(float FinalDamage)
{
	if (!ensureAlways(IsValid(CachedOwnerCharacter)) || !CachedOwnerCharacter->HasAuthority())
	{
		return;
	}

	const ANetherCrownPlayerState* OwnerPlayerState{ Cast<ANetherCrownPlayerState>(CachedOwnerCharacter->GetPlayerState()) };
	if (!ensureAlways(IsValid(OwnerPlayerState)))
	{
		return;
	}

	UNetherCrownPlayerStatComponent* PlayerStatComponent{ OwnerPlayerState->GetNetherCrownPlayerStatComponent() };
	if (!ensureAlways(IsValid(PlayerStatComponent)))
	{
		return;
	}

	PlayerStatComponent->ModifyHp(-FinalDamage);
}

int32 UNetherCrownDamageReceiverComponent::GetArmorStat(const bool bIsPhysicalDamage) const
{
	if (!ensureAlways(IsValid(CachedOwnerCharacter)) || !CachedOwnerCharacter->HasAuthority())
	{
		return 0;
	}

	const ANetherCrownPlayerState* OwnerPlayerState{ Cast<ANetherCrownPlayerState>(CachedOwnerCharacter->GetPlayerState()) };
	if (!ensureAlways(IsValid(OwnerPlayerState)))
	{
		return 0;
	}


	const UNetherCrownPlayerStatComponent* PlayerStatComponent{ OwnerPlayerState->GetNetherCrownPlayerStatComponent() };
	if (!ensureAlways(IsValid(PlayerStatComponent)))
	{
		return 0;
	}

	const FNetherCrownPlayerStat& PlayerStat{ PlayerStatComponent->GetPlayerStatData() };

	return bIsPhysicalDamage ? PlayerStat.PhysicalArmor : PlayerStat.MagicArmor;
}

bool UNetherCrownDamageReceiverComponent::IsDead() const
{
	if (!ensureAlways(IsValid(CachedOwnerCharacter)) || !CachedOwnerCharacter->HasAuthority())
	{
		return false;
	}

	const ANetherCrownPlayerState* OwnerPlayerState{ Cast<ANetherCrownPlayerState>(CachedOwnerCharacter->GetPlayerState()) };
	if (!ensureAlways(IsValid(OwnerPlayerState)))
	{
		return false;
	}


	const UNetherCrownPlayerStatComponent* PlayerStatComponent{ OwnerPlayerState->GetNetherCrownPlayerStatComponent() };
	if (!ensureAlways(IsValid(PlayerStatComponent)))
	{
		return false;
	}

	const FNetherCrownPlayerStat& PlayerStat{ PlayerStatComponent->GetPlayerStatData() };
	const int32 CurrentHP{ PlayerStat.CharacterHP };

	return CurrentHP <= 0;
}
