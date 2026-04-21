// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyBasicAttackComponent.h"

#include "Net/UnrealNetwork.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownEnemyStatComponent.h"
#include "NetherCrown/Data/NetherCrownEnemySkillData.h"
#include "NetherCrown/Enemy/AnimInstance/NetherCrownEnemyAnimInstance.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/Util/NetherCrownCollisionChannels.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"
#include "NetherCrown/Weapon/NetherCrownEnemyWeapon.h"

UNetherCrownEnemyBasicAttackComponent::UNetherCrownEnemyBasicAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UNetherCrownEnemyBasicAttackComponent::SetHandledEnemyWeapon(ANetherCrownEnemyWeapon* InWeapon)
{
	HandledEnemyWeaponWeak = MakeWeakObjectPtr(InWeapon);
}

bool UNetherCrownEnemyBasicAttackComponent::TryGetKnockBackData(float& OutKnockBackDistance, float& OutKnockBackDuration) const
{
	OutKnockBackDistance = 0.f;
	OutKnockBackDuration = 0.f;

	if (ActiveEnemyBasicAttackData.BasicAttackType != ENetherCrownEnemyBasicAttackType::DashAttack)
	{
		return false;
	}

	const UNetherCrownEnemyDashAttackDataAsset* EnemyDashAttackDataAsset{ Cast<UNetherCrownEnemyDashAttackDataAsset>(FNetherCrownUtilManager::GetEnemySkillDataAssetByGameplayTag(NetherCrownTags::Enemy_Skill_DashAttack)) };
	if (!ensureAlways(IsValid(EnemyDashAttackDataAsset)))
	{
		return false;
	}

	const FNetherCrownEnemyDashAttackData& EnemyDashAttackData{ EnemyDashAttackDataAsset->GetEnemyDashAttackData() };
	OutKnockBackDistance = EnemyDashAttackData.DashKnockBackDistance;
	OutKnockBackDuration = EnemyDashAttackData.DashKnockBackDuration;

	return true;
}

void UNetherCrownEnemyBasicAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedOwnerEnemy = Cast<ANetherCrownEnemy>(GetOwner());
	LoadEnemyBasicAttackData();
	CacheBasicAttackAnimMontage();

	ActiveEnemyBasicAttackData = EnemyBasicAttackData;
	CachedActiveAttackMontage = CachedBasicAttackMontage;
}

void UNetherCrownEnemyBasicAttackComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bEnableHitTrace)
	{
		DetectHit();
	}
}

void UNetherCrownEnemyBasicAttackComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bEnableHitTrace);
	DOREPLIFETIME(ThisClass, EnemyBasicAttackState);
	DOREPLIFETIME(ThisClass, LastEndLocation);
	DOREPLIFETIME(ThisClass, HitIgnoreCharacters);
}

int32 UNetherCrownEnemyBasicAttackComponent::GetEnemyAttackDamage() const
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)))
	{
		return 0;
	}

	const UNetherCrownEnemyStatComponent* EnemyStatComponent{ CachedOwnerEnemy->GetEnemyStatComponent() };
	if (!ensureAlways(IsValid(EnemyStatComponent)))
	{
		return 0;
	}

	return EnemyStatComponent->GetEnemyStatData().AttackDamage;
}

void UNetherCrownEnemyBasicAttackComponent::SetupBasicAttackTimer(const FNetherCrownEnemyBasicAttackComboData& EnemyBasicAttackComboData)
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

	TimerManager.ClearTimer(EnableHitTraceTimerHandle);
	TimerManager.ClearTimer(DisableHitTraceTimerHandle);
	TimerManager.ClearTimer(AttackEndTimerHandle);

	TimerManager.SetTimer(EnableHitTraceTimerHandle, this, &ThisClass::EnableHitTrace, EnemyBasicAttackComboData.EnableHitTraceTime, false);
	TimerManager.SetTimer(DisableHitTraceTimerHandle, this, &ThisClass::DisableHitTrace, EnemyBasicAttackComboData.DisableHitTraceTime, false);
	TimerManager.SetTimer(AttackEndTimerHandle, this, &ThisClass::EndAttack, EnemyBasicAttackComboData.EndAttackTime, false);
}

void UNetherCrownEnemyBasicAttackComponent::EnableHitTrace()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	Multicast_InitHitTraceState(GetWeaponTraceSocketLocation());
	bEnableHitTrace = true;
}

void UNetherCrownEnemyBasicAttackComponent::DisableHitTrace()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	bEnableHitTrace = false;
}

void UNetherCrownEnemyBasicAttackComponent::EndAttack()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	EnemyBasicAttackState = ENetherCrownEnemyBasicAttackState::NotAttacking;
}

void UNetherCrownEnemyBasicAttackComponent::DetectHit()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || CachedOwnerEnemy->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	const FVector& CurrentEndLocation{ GetWeaponTraceSocketLocation() };

	TArray<FHitResult> HitResults{};
	FCollisionQueryParams Params{};
	Params.AddIgnoredActor(CachedOwnerEnemy);

	for (const ANetherCrownCharacter* HitCharacter : HitIgnoreCharacters)
	{
		Params.AddIgnoredActor(HitCharacter);
	}

	//@NOTE : bHit는 block일때만 true가 됨, overlap일때는 false
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		LastEndLocation,
		CurrentEndLocation,
		FQuat::Identity,
		ECC_WeaponHitCheck,
		FCollisionShape::MakeSphere(ActiveEnemyBasicAttackData.TraceRadius),
		Params
	);

#if 1
	DrawDebugLine(GetWorld(), LastEndLocation, CurrentEndLocation, FColor::Red, false, 2.0f);
	DrawDebugSphere(GetWorld(), LastEndLocation, ActiveEnemyBasicAttackData.TraceRadius, 8, FColor::Yellow, false, 2.0f);
	DrawDebugSphere(GetWorld(), CurrentEndLocation, ActiveEnemyBasicAttackData.TraceRadius, 8, FColor::Blue, false, 2.0f);
#endif

	LastEndLocation = CurrentEndLocation;

	if (HitResults.IsEmpty())
	{
		return;
	}

	TMap<ANetherCrownCharacter*, FVector> HitInfoMap{};
	for (const FHitResult& Hit : HitResults)
	{
		if (ANetherCrownCharacter* HitCharacter = Cast<ANetherCrownCharacter>(Hit.GetActor()))
		{
			HitInfoMap.Add(HitCharacter, Hit.ImpactPoint);
			HitIgnoreCharacters.AddUnique(HitCharacter);
		}
	}

	for (const TPair<ANetherCrownCharacter*, FVector>& HitInfo : HitInfoMap)
	{
		ANetherCrownCharacter* HitCharacter{ HitInfo.Key };
		if (ensureAlways(IsValid(HitCharacter)))
		{
			HitCharacter->Server_ReportHitBasicAttackByEnemy(CachedOwnerEnemy, GetEnemyAttackDamage());
		}
	}
}

void UNetherCrownEnemyBasicAttackComponent::LoadEnemyBasicAttackData()
{
	if (EnemyBasicAttackDataAssetSoft.IsNull())
	{
		return;
	}

	const UNetherCrownEnemyBasicAttackDataAsset* EnemyBasicAttackDataAsset{ EnemyBasicAttackDataAssetSoft.LoadSynchronous() };
	if (!ensureAlways(IsValid(EnemyBasicAttackDataAsset)))
	{
		return;
	}

	EnemyBasicAttackData = EnemyBasicAttackDataAsset->GetEnemyBasicAttackData();
}

void UNetherCrownEnemyBasicAttackComponent::CacheBasicAttackAnimMontage()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || CachedOwnerEnemy->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (EnemyBasicAttackData.BasicAttackMontageSoft.IsNull())
	{
		return;
	}

	CachedBasicAttackMontage = EnemyBasicAttackData.BasicAttackMontageSoft.LoadSynchronous();
}

FVector UNetherCrownEnemyBasicAttackComponent::GetWeaponTraceSocketLocation() const
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)))
	{
		return FVector::ZeroVector;
	}

	const ANetherCrownEnemyWeapon* HandledEnemyWeapon{ HandledEnemyWeaponWeak.Get() };
	const USkeletalMeshComponent* WeaponOrBodySkeletalMeshComponent{ HandledEnemyWeapon ? HandledEnemyWeapon->GetEnemyWeaponSkeletalMeshComponent() : CachedOwnerEnemy->GetMesh() };
	if (!ensureAlways(IsValid(WeaponOrBodySkeletalMeshComponent)))
	{
		return FVector::ZeroVector;
	}

	return WeaponOrBodySkeletalMeshComponent->GetSocketLocation(ActiveEnemyBasicAttackData.WeaponTraceSocketName);
}

void UNetherCrownEnemyBasicAttackComponent::RequestEnemyAttack()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	StartEnemyAttack(EnemyBasicAttackData, CachedBasicAttackMontage);
}

void UNetherCrownEnemyBasicAttackComponent::RequestEnemyAttackByDA(const UNetherCrownEnemyBasicAttackDataAsset* InEnemyBasicAttackDataAsset)
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	if (!ensureAlways(IsValid(InEnemyBasicAttackDataAsset)))
	{
		return;
	}

	const FNetherCrownEnemyBasicAttackData DashEnemyBasicAttackData{ InEnemyBasicAttackDataAsset->GetEnemyBasicAttackData() };
	UAnimMontage* DashEnemyBasicAttackMontage{ DashEnemyBasicAttackData.BasicAttackMontageSoft.IsNull() ? nullptr : DashEnemyBasicAttackData.BasicAttackMontageSoft.LoadSynchronous() };

	StartEnemyAttack(DashEnemyBasicAttackData, DashEnemyBasicAttackMontage);
}

void UNetherCrownEnemyBasicAttackComponent::StartEnemyAttack(const FNetherCrownEnemyBasicAttackData& InEnemyBasicAttackData, UAnimMontage* InAttackMontage, const bool bNeedAttackAnimMontage/*true*/)
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	if (EnemyBasicAttackState != ENetherCrownEnemyBasicAttackState::Attacking)
	{
		ActiveEnemyBasicAttackData = InEnemyBasicAttackData;
		CachedActiveAttackMontage = InAttackMontage;
		LastEndLocation = GetWeaponTraceSocketLocation();

		EnemyBasicAttackState = ENetherCrownEnemyBasicAttackState::Attacking;

		const TMap<int32, FNetherCrownEnemyBasicAttackComboData>& EnemyComboDataMap{ ActiveEnemyBasicAttackData.EnemyComboDataMap };
		if (EnemyComboDataMap.IsEmpty())
		{
			return;
		}

		const int32 ComboDataIndex{ EnemyComboDataMap.Num() == 1 ? 0 : FMath::RandRange(0, EnemyComboDataMap.Num() - 1) };
		SetupBasicAttackTimer(EnemyComboDataMap[ComboDataIndex]);

		if (bNeedAttackAnimMontage)
		{
			Multicast_PlayBasicAttackMontage(EnemyComboDataMap[ComboDataIndex].ComboMontageSectionName);
		}
	}
}

void UNetherCrownEnemyBasicAttackComponent::Multicast_InitHitTraceState_Implementation(const FVector& InLastEndLocation)
{
	HitIgnoreCharacters.Empty();
	LastEndLocation = InLastEndLocation;
}

void UNetherCrownEnemyBasicAttackComponent::Multicast_PlayBasicAttackMontage_Implementation(const FName& ComboMontageSectionName)
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || CachedOwnerEnemy->GetNetMode() == NM_DedicatedServer)
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

	if (!ensureAlways(IsValid(CachedActiveAttackMontage)))
	{
		return;
	}

	EnemyAnimInstance->Montage_Play(CachedActiveAttackMontage);
	EnemyAnimInstance->Montage_JumpToSection(ComboMontageSectionName, CachedActiveAttackMontage);
}
