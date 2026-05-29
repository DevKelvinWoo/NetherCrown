// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownEnemyDamageReceiverComponent.h"

#include "NetherCrownCrowdControlComponent.h"
#include "Engine/DamageEvents.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "NetherCrownCrowdControlTypes.h"
#include "NetherCrownEnemyStatComponent.h"
#include "NetherCrownEquipComponent.h"
#include "NetherCrownQuestComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/DamageTypes/UNetherCrownCriticalPhysicalDamageType.h"
#include "NetherCrown/DamageTypes/NetherCrownDamageEvent.h"
#include "NetherCrown/DamageTypes/NetherCrownPhysicalDamageType.h"
#include "NetherCrown/Data/NetherCrownWeaponData.h"
#include "NetherCrown/Effects/NetherCrownDamageNumberActor.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Enemy/AIController/NetherCrownEnemyAIController.h"
#include "NetherCrown/Enemy/AnimInstance/NetherCrownEnemyAnimInstance.h"
#include "NetherCrown/Projectile/NetherCrownEnemyMagicProjectile.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

UNetherCrownEnemyDamageReceiverComponent::UNetherCrownEnemyDamageReceiverComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void UNetherCrownEnemyDamageReceiverComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheOwnerEnemy();
	LoadEnemyDamageCosmeticData();
	CacheDeathMaterialInstances();
	BindTimelineFunctions();
}

void UNetherCrownEnemyDamageReceiverComponent::TickComponent(const float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (DeathMaterialParamTimeline.IsPlaying())
	{
		DeathMaterialParamTimeline.TickTimeline(DeltaTime);
	}
}

void UNetherCrownEnemyDamageReceiverComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EnemyHitReactState);
}

void UNetherCrownEnemyDamageReceiverComponent::HandleEnemyDeadQuest(AActor* DamageCauser)
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	ANetherCrownCharacter* DamageCauserCharacter{ Cast<ANetherCrownCharacter>(DamageCauser) };
	if (!ensureAlways(IsValid(DamageCauserCharacter)))
	{
		return;
	}

	UNetherCrownQuestComponent* QuestComponent{ DamageCauserCharacter->GetQuestComponent() };
	if (!ensureAlways(IsValid(QuestComponent)))
	{
		return;
	}

	QuestComponent->CheckAndAddQuestCountProgress(CachedOwnerEnemy->GetEnemyTag(), 1);
}

float UNetherCrownEnemyDamageReceiverComponent::HandleIncomingDamage(const float DamageAmount, FDamageEvent const& DamageEvent, AActor* DamageCauser)
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return 0.f;
	}

	SetHitReactStateAndTimer();

	const float FinalDamage{ CalculateFinalDamage(DamageAmount, DamageEvent, DamageCauser) };
	const bool bIsCriticalDamage{ DamageEvent.DamageTypeClass == UNetherCrownCriticalPhysicalDamageType::StaticClass() };
	ApplyFinalDamage(FinalDamage);
	Multicast_SpawnDamageNumber(FMath::RoundToInt(FinalDamage), bIsCriticalDamage);

	if (IsDead())
	{
		HandleEnemyDead(DamageCauser);
	}
	else
	{
		Multicast_PlayTakeDamageAnimation(bIsCriticalDamage);
		Multicast_PlayTakeDamageSound(ResolveHurtImpactSoundTag(DamageEvent));
	}

	return FinalDamage;
}

float UNetherCrownEnemyDamageReceiverComponent::CalculateFinalDamage(const float DamageAmount, FDamageEvent const& DamageEvent, const AActor* DamageCauser) const
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return 0.f;
	}

	int32 FinalDamage{};
	if (const ANetherCrownEnemyMagicProjectile* EnemyMagicProjectile = Cast<ANetherCrownEnemyMagicProjectile>(DamageCauser))
	{
		FinalDamage = EnemyMagicProjectile->GetProjectileDamage();
		return FinalDamage;
	}

	const bool bIsPhysicalDamage{ DamageEvent.DamageTypeClass->IsChildOf(UNetherCrownPhysicalDamageType::StaticClass()) };

	const int32 ArmorStat{ GetArmorStat(bIsPhysicalDamage) };

	const int32 EffectiveArmor{ FMath::Max(0, ArmorStat - GetWeaponPenetration(bIsPhysicalDamage, DamageCauser)) };

	const float DamageMultiplier{ 100.f / (100.f + EffectiveArmor) };
	FinalDamage = FMath::RoundToInt(DamageAmount * DamageMultiplier);

	return FinalDamage;
}

void UNetherCrownEnemyDamageReceiverComponent::ApplyFinalDamage(const float FinalDamage)
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

	EnemyStatComponent->ModifyEnemyHp(-FinalDamage);
}

void UNetherCrownEnemyDamageReceiverComponent::SetHitReactStateAndTimer()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	const UNetherCrownCrowdControlComponent* CCComponent{ CachedOwnerEnemy->GetCrowdControlComponent() };
	if (!ensureAlways(IsValid(CCComponent)))
	{
		ClearHitReactState();
		return;
	}

	if (CCComponent->IsCrowdControlActive())
	{
		ClearHitReactState();
		return;
	}

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		ClearHitReactState();
		return;
	}

	FTimerManager& TimerManager{ World->GetTimerManager() };
	TimerManager.ClearTimer(HitReactTimerHandle);

	if (EnemyDamageCosmeticData.HitReactionDuration <= 0.f)
	{
		ClearHitReactState();
		return;
	}

	EnemyHitReactState = ENetherCrownEnemyHitReactState::HitReact;
	TimerManager.SetTimer(HitReactTimerHandle, this, &ThisClass::ClearHitReactState, EnemyDamageCosmeticData.HitReactionDuration, false);
}

void UNetherCrownEnemyDamageReceiverComponent::ClearHitReactState()
{
	EnemyHitReactState = ENetherCrownEnemyHitReactState::None;

	if (const UWorld* World{ GetWorld() })
	{
		World->GetTimerManager().ClearTimer(HitReactTimerHandle);
	}
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

void UNetherCrownEnemyDamageReceiverComponent::HandleEnemyDead(AActor* DamageCauser)
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	HandleEnemyDeadQuest(DamageCauser);

	ANetherCrownEnemyAIController* EnemyAIController{ Cast<ANetherCrownEnemyAIController>(CachedOwnerEnemy->GetController()) };
	if (!ensureAlways(IsValid(EnemyAIController)))
	{
		return;
	}

	EnemyAIController->StopMovement();
	EnemyAIController->HandleEnemyDead();
	CachedOwnerEnemy->SetIsDead(true);

	Multicast_PlayDeathSound();

	UCharacterMovementComponent* MovementComponent{ CachedOwnerEnemy->GetCharacterMovement() };
	if (!ensureAlways(IsValid(MovementComponent)))
	{
		return;
	}

	MovementComponent->StopMovementImmediately();
	MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
	CachedOwnerEnemy->LaunchCharacter(-CachedOwnerEnemy->GetActorForwardVector() * EnemyDeathCosmeticData.DeathKnockBackDistance, true, true);

	const UNetherCrownCrowdControlComponent* CCComponent{ CachedOwnerEnemy->GetCrowdControlComponent() };
	if (!ensureAlways(IsValid(CCComponent)))
	{
		return;
	}

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	const bool bIsFrozen{ CCComponent->GetCrowdControlType() == ENetherCrownCrowdControlType::FROZEN };
	if (bIsFrozen)
	{
		Multicast_SpawnDeathEffectAndSound();
		CachedOwnerEnemy->Destroy();
	}
	else
	{
		Multicast_StartDeathDissolve();

		const float DeathTimingOffset{ EnemyDeathCosmeticData.DestroyTimeOffset };
		World->GetTimerManager().ClearTimer(HandleDestroyTimerHandle);
		World->GetTimerManager().SetTimer(HandleDestroyTimerHandle, this, &ThisClass::HandleDeathTimer, DeathTimingOffset, false);
	}
}

int32 UNetherCrownEnemyDamageReceiverComponent::GetWeaponPenetration(const bool bIsPhysicalDamage, const AActor* DamageCauser) const
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return 0;
	}

	const ANetherCrownCharacter* NetherCrownCharacter = Cast<ANetherCrownCharacter>(DamageCauser);
	if (!IsValid(NetherCrownCharacter))
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

FGameplayTag UNetherCrownEnemyDamageReceiverComponent::ResolveHurtImpactSoundTag(FDamageEvent const& DamageEvent) const
{
	const FNetherCrownDamageEvent& NetherCrownDamageEvent{ static_cast<const FNetherCrownDamageEvent&>(DamageEvent) };
	if (NetherCrownDamageEvent.HitImpactSoundTag.IsValid())
	{
		return NetherCrownDamageEvent.HitImpactSoundTag;
	}

	const UNetherCrownCrowdControlComponent* CCComponent{ CachedOwnerEnemy->GetCrowdControlComponent() };
	if (!ensureAlways(IsValid(CCComponent)))
	{
		return FGameplayTag{};
	}

	const bool bIsFrozen{ CCComponent->GetCrowdControlType() == ENetherCrownCrowdControlType::FROZEN };
	const FNetherCrownEnemyDamageTagData& EnemyDamageTagData{ EnemyDamageCosmeticData.EnemyDamageTagData };
	return bIsFrozen ? EnemyDamageTagData.FrozenEnemyHurtImpactSoundTag : EnemyDamageTagData.EnemyHurtImpactSoundTag;
}

void UNetherCrownEnemyDamageReceiverComponent::CacheOwnerEnemy()
{
	CachedOwnerEnemy = Cast<ANetherCrownEnemy>(GetOwner());
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

	if (!(EnemyDeathCosmeticData.DestroyMaterialParamCurveFloatSoft.IsNull()))
	{
		CachedDeathMaterialParamCurveFloat = EnemyDeathCosmeticData.DestroyMaterialParamCurveFloatSoft.LoadSynchronous();
	}
}

void UNetherCrownEnemyDamageReceiverComponent::CacheDeathMaterialInstances()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || CachedOwnerEnemy->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	USkeletalMeshComponent* EnemyMesh{ CachedOwnerEnemy->GetMesh() };
	if (!ensureAlways(IsValid(EnemyMesh)))
	{
		return;
	}

	CachedDeathMaterialInstances.Reset();

	const int32 MaterialNum{ EnemyMesh->GetNumMaterials() };
	for (int32 MaterialIndex{ 0 }; MaterialIndex < MaterialNum; ++MaterialIndex)
	{
		UMaterialInstanceDynamic* DynamicMaterial{ EnemyMesh->CreateDynamicMaterialInstance(MaterialIndex) };
		if (!IsValid(DynamicMaterial))
		{
			continue;
		}

		DynamicMaterial->SetScalarParameterValue(EnemyDeathCosmeticData.DestroyMaterialParamName, 0.f);
		CachedDeathMaterialInstances.Add(DynamicMaterial);
	}
}

void UNetherCrownEnemyDamageReceiverComponent::BindTimelineFunctions()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	FOnTimelineFloat OnTimelineFloat{};
	OnTimelineFloat.BindUFunction(this, FName("ApplyDeadMaterialParam"));
	DeathMaterialParamTimeline.AddInterpFloat(CachedDeathMaterialParamCurveFloat, OnTimelineFloat);
}

void UNetherCrownEnemyDamageReceiverComponent::Multicast_PlayDeathSound_Implementation()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || CachedOwnerEnemy->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	FNetherCrownUtilManager::PlaySound2DByGameplayTag(this, EnemyDeathCosmeticData.EnemyDeathTagData.EnemyDeathSoundTag);
}

void UNetherCrownEnemyDamageReceiverComponent::Multicast_StartDeathDissolve_Implementation()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	DeathMaterialParamTimeline.PlayFromStart();
}

void UNetherCrownEnemyDamageReceiverComponent::Multicast_SpawnDeathEffectAndSound_Implementation()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	const FTransform SpawnTransform{ FRotator::ZeroRotator, CachedOwnerEnemy->GetActorLocation(), FVector(1.0f) };

	UNetherCrownCrowdControlComponent* CCComponent{ CachedOwnerEnemy->GetCrowdControlComponent() };
	if (!ensureAlways(IsValid(CCComponent)))
	{
		return;
	}

	const ENetherCrownCrowdControlType CCType{ CCComponent->GetCrowdControlType() };
	if (CCType == ENetherCrownCrowdControlType::FROZEN)
	{
		FNetherCrownUtilManager::SpawnNiagaraSystemByGameplayTag(CachedOwnerEnemy, EnemyDeathCosmeticData.EnemyDeathTagData.FrozenEnemyDeathEffectTag, SpawnTransform);
		FNetherCrownUtilManager::SpawnNiagaraSystemByGameplayTag(CachedOwnerEnemy, EnemyDeathCosmeticData.EnemyDeathTagData.FrozenEnemyDeathEffectBackGround, SpawnTransform);

		FNetherCrownUtilManager::PlaySound2DByGameplayTag(CachedOwnerEnemy, EnemyDeathCosmeticData.EnemyDeathTagData.FrozenEnemyDeathSoundTag);
	}
}

void UNetherCrownEnemyDamageReceiverComponent::Multicast_SpawnDamageNumber_Implementation(const int32 DamageAmount, const bool bIsCriticalDamage)
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || CachedOwnerEnemy->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (!DamageNumberActorClass || DamageAmount <= 0)
	{
		return;
	}

	UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	FVector SpawnOffset{ DamageNumberSpawnOffset };
	if (DamageNumberRandomHorizontalOffset > 0.f)
	{
		SpawnOffset.X += FMath::RandRange(-DamageNumberRandomHorizontalOffset, DamageNumberRandomHorizontalOffset);
		SpawnOffset.Y += FMath::RandRange(-DamageNumberRandomHorizontalOffset, DamageNumberRandomHorizontalOffset);
	}

	FActorSpawnParameters SpawnParameters{};
	SpawnParameters.Owner = CachedOwnerEnemy;
	SpawnParameters.ObjectFlags = RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ANetherCrownDamageNumberActor* DamageNumberActor{ World->SpawnActor<ANetherCrownDamageNumberActor>(DamageNumberActorClass, CachedOwnerEnemy->GetActorLocation() + SpawnOffset, FRotator::ZeroRotator, SpawnParameters) };
	if (!ensureAlways(IsValid(DamageNumberActor)))
	{
		return;
	}

	DamageNumberActor->InitDamageNumber(DamageAmount, bIsCriticalDamage);
}

void UNetherCrownEnemyDamageReceiverComponent::ApplyDeadMaterialParam(const float FloatCurveValue)
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (CachedDeathMaterialInstances.IsEmpty())
	{
		return;
	}

	for (UMaterialInstanceDynamic* DynamicMaterial : CachedDeathMaterialInstances)
	{
		if (!IsValid(DynamicMaterial))
		{
			continue;
		}

		DynamicMaterial->SetScalarParameterValue(TEXT("DissolveAmount"), FloatCurveValue);
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

	if (!IsValid(CachedTakeDamageAnimMontage) || !IsValid(CachedTakeCriticalDamageAnimMontage))
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

void UNetherCrownEnemyDamageReceiverComponent::Multicast_PlayTakeDamageSound_Implementation(const FGameplayTag& HurtImpactSoundTag)
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || CachedOwnerEnemy->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	const UNetherCrownCrowdControlComponent* CCComponent{ CachedOwnerEnemy->GetCrowdControlComponent() };
	if (!ensureAlways(IsValid(CCComponent)))
	{
		return;
	}

	const bool bIsFrozen{ CCComponent->GetCrowdControlType() == ENetherCrownCrowdControlType::FROZEN };
	const FNetherCrownEnemyDamageTagData& EnemyDamageTagData{ EnemyDamageCosmeticData.EnemyDamageTagData };

	if (!bIsFrozen)
	{
		FNetherCrownUtilManager::PlaySound2DByGameplayTag(this, EnemyDamageTagData.EnemyHurtGruntSoundTag);
	}

	FNetherCrownUtilManager::PlaySound2DByGameplayTag(this, HurtImpactSoundTag);
}
