// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownEnemyMagicSpike.h"

#include "NavigationSystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/NetherCrown.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownCrowdControlComponent.h"
#include "NetherCrown/DamageTypes/NetherCrownDamageEvent.h"
#include "NetherCrown/DamageTypes/NetherCrownMagicDamageType.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/Util/NetherCrownCollisionChannels.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

void UNetherCrownEnemyMagicSpike::InitEnemySkillObject()
{
	Super::InitEnemySkillObject();

	CacheMagicSpikeData();
}

void UNetherCrownEnemyMagicSpike::PlayEnemySkillCosmetics()
{
	Super::PlayEnemySkillCosmetics();
}

void UNetherCrownEnemyMagicSpike::ExecuteEnemySkillGameplay()
{
	Super::ExecuteEnemySkillGameplay();

	CurrentMagicSpikeCount = 0;
	ShowMagicSpikeRange();
	StartMagicSpikeRangeTimer();
}

void UNetherCrownEnemyMagicSpike::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentMagicSpikeCount);
}

void UNetherCrownEnemyMagicSpike::CacheMagicSpikeData()
{
	const UNetherCrownEnemyMagicSpikeDataAsset* MagicSpikeDataAsset{ Cast<UNetherCrownEnemyMagicSpikeDataAsset>(FNetherCrownUtilManager::GetEnemySkillDataAssetByGameplayTag(NetherCrownTags::Enemy_Skill_MagicSpike)) };
	if (!ensureAlways(IsValid(MagicSpikeDataAsset)))
	{
		return;
	}

	CachedMagicSpikeData = MagicSpikeDataAsset->GetEnemyMagicSpikeData();
}

FVector UNetherCrownEnemyMagicSpike::GetMagicSpikePoint() const
{
	const ANetherCrownEnemy* OwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(OwnerEnemy)) || !OwnerEnemy->HasAuthority())
	{
		return {};
	}

	const ANetherCrownCharacter* CurrentTargetCharacter{ OwnerEnemy->GetCurrentTargetCharacter() };
	if (!IsValid(CurrentTargetCharacter))
	{
		return {};
	}

	const UNavigationSystemV1* NavSystem{ UNavigationSystemV1::GetNavigationSystem(GetWorld()) };
	if (!ensureAlways(IsValid(NavSystem)))
	{
		return {};
	}

	const FVector& PointVector{ CurrentTargetCharacter->GetActorLocation() };
	FNavLocation PointLocation{};
	bool bProjected{ NavSystem->ProjectPointToNavigation(PointVector, PointLocation) };
	if (!bProjected)
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("Failed to project point to navigation"));
		return{};
	}

	FNavLocation RandomPointLocation{};
	bool bFoundRandomReachablePoint{ NavSystem->GetRandomReachablePointInRadius(PointLocation.Location, CachedMagicSpikeData.MagicSpikeSpawnRadius, RandomPointLocation) };
	if (!bFoundRandomReachablePoint)
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("Failed to find random reachable point"));
		return {};
	}

	return RandomPointLocation.Location;
}

void UNetherCrownEnemyMagicSpike::DetectAndApplyMagicSpikeDamage(const FVector& InMagicSpikePointVector)
{
	const ANetherCrownEnemy* OwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(OwnerEnemy)) || !OwnerEnemy->HasAuthority())
	{
		return;
	}

	TArray<AActor*> OverlappedActors{};
	const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes{ UEngineTypes::ConvertToObjectType(ECC_Player) };
	UKismetSystemLibrary::SphereOverlapActors(this, InMagicSpikePointVector, CachedMagicSpikeData.MagicSpikeRadius, ObjectTypes, ANetherCrownCharacter::StaticClass(),
		TArray<AActor*>(), OverlappedActors);

	for (AActor* OverlappedActor : OverlappedActors)
	{
		ANetherCrownCharacter* OverlappedCharacter = Cast<ANetherCrownCharacter>(OverlappedActor);
		if (!ensureAlways(IsValid(OverlappedCharacter)))
		{
			continue;
		}

		ApplyEnemyMagicSkillDamage(OverlappedCharacter);

		UNetherCrownCrowdControlComponent* CCComponent{ OverlappedCharacter->GetCrowdControlComponent() };
		if (!ensureAlways(IsValid(CCComponent)))
		{
			continue;
		}

		CCComponent->ApplyCrowdControl(ENetherCrownCrowdControlType::STUN, CachedMagicSpikeData.MagicSpikeStunDuration);
		CCComponent->Stun();
	}
}

void UNetherCrownEnemyMagicSpike::StartMagicSpikeRangeTimer()
{
	const ANetherCrownEnemy* OwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(OwnerEnemy)) || !OwnerEnemy->HasAuthority())
	{
		return;
	}

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	FTimerManager& TimerManager{ World->GetTimerManager() };
	TimerManager.SetTimer(MagicSpikeRangeTimerHandle, this, &ThisClass::ShowMagicSpikeRange, CachedMagicSpikeData.MagicSpikeRangeInterval, true);
}

void UNetherCrownEnemyMagicSpike::StartMagicSpikeAttackTimer(const FVector& InMagicSpikePointVector)
{
	const ANetherCrownEnemy* OwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(OwnerEnemy)) || !OwnerEnemy->HasAuthority())
	{
		return;
	}

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	FTimerDelegate TimerDelegate{};
	TimerDelegate.BindUObject(this, &ThisClass::ShowMagicSpikeImpactAndApplyDamage, InMagicSpikePointVector);

	FTimerManager& TimerManager{ World->GetTimerManager() };
	FTimerHandle AttackTimerHandle{};
	TimerManager.SetTimer(AttackTimerHandle, TimerDelegate, CachedMagicSpikeData.MagicSpikeImpactTimeOffset, false);
}

void UNetherCrownEnemyMagicSpike::ShowMagicSpikeRange()
{
	const ANetherCrownEnemy* OwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(OwnerEnemy)) || !OwnerEnemy->HasAuthority())
	{
		return;
	}

	if (CurrentMagicSpikeCount >= CachedMagicSpikeData.MagicSpikeMaxCount)
	{
		const UWorld* World{ GetWorld() };
		if (!ensureAlways(IsValid(World)))
		{
			return;
		}

		World->GetTimerManager().ClearTimer(MagicSpikeRangeTimerHandle);

		return;
	}

	++CurrentMagicSpikeCount;

	const FVector MagicSpikePointVector{ GetMagicSpikePoint() };

	const FNetherCrownMagicSpikeTagData& MagicSpikeTagData{ CachedMagicSpikeData.MagicSpikeTagData };
	Multicast_ShowMagicSpikeEffectAndPlaySound(MagicSpikeTagData.RangeEffectTag, MagicSpikeTagData.RangeSoundTag, MagicSpikePointVector);

	StartMagicSpikeAttackTimer(MagicSpikePointVector);
}

void UNetherCrownEnemyMagicSpike::ShowMagicSpikeImpactAndApplyDamage(FVector InMagicSpikePointVector)
{
	const ANetherCrownEnemy* OwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(OwnerEnemy)) || !OwnerEnemy->HasAuthority())
	{
		return;
	}

	const FNetherCrownMagicSpikeTagData& MagicSpikeTagData{ CachedMagicSpikeData.MagicSpikeTagData };
	Multicast_ShowMagicSpikeEffectAndPlaySound(MagicSpikeTagData.SpikeEffectTag, MagicSpikeTagData.SpikeSoundTag, InMagicSpikePointVector);

	DetectAndApplyMagicSpikeDamage(InMagicSpikePointVector);
}

void UNetherCrownEnemyMagicSpike::Multicast_ShowMagicSpikeEffectAndPlaySound_Implementation(const FGameplayTag& InEffectTag, const FGameplayTag& InSoundTag, const FVector& InMagicSpikePointVector)
{
	const ANetherCrownEnemy* OwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(OwnerEnemy)) || OwnerEnemy->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	const FTransform& RangeEffectTransform{ FTransform(FRotator::ZeroRotator, InMagicSpikePointVector, FVector::OneVector) };
	FNetherCrownUtilManager::SpawnNiagaraSystemByGameplayTag(this, InEffectTag, RangeEffectTransform);
	FNetherCrownUtilManager::PlaySound2DByGameplayTag(this, InSoundTag);
}
