// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyBasicAttackComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/DamageTypes/NetherCrownPhysicalDamageType.h"
#include "NetherCrown/Enemy/AnimInstance/NetherCrownEnemyAnimInstance.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Util/NetherCrownCollisionChannels.h"
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

void UNetherCrownEnemyBasicAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedOwnerEnemy = Cast<ANetherCrownEnemy>(GetOwner());
	LoadEnemyBasicAttackData();
	CacheBasicAttackAnimMontage();
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
}

void UNetherCrownEnemyBasicAttackComponent::SetupBasicAttackTimer()
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

	TimerManager.SetTimer(EnableHitTraceTimerHandle, this, &ThisClass::EnableHitTrace, EnemyBasicAttackData.EnableHitTraceTime, false);
	TimerManager.SetTimer(DisableHitTraceTimerHandle, this, &ThisClass::DisableHitTrace, EnemyBasicAttackData.DisableHitTraceTime, false);
	TimerManager.SetTimer(AttackEndTimerHandle, this, &ThisClass::EndAttack, EnemyBasicAttackData.EndAttackTime, false);
}

void UNetherCrownEnemyBasicAttackComponent::EnableHitTrace()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

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

void UNetherCrownEnemyBasicAttackComponent::Server_ReportHit_Implementation(ANetherCrownCharacter* HitCharacter, const FVector& HitLocation)
{
	//@TODO : 데미지 계산 로직 필요
	UGameplayStatics::ApplyDamage(HitCharacter, 10.f, CachedOwnerEnemy->GetInstigatorController(), CachedOwnerEnemy, UNetherCrownPhysicalDamageType::StaticClass());
}

void UNetherCrownEnemyBasicAttackComponent::DetectHit()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	const FVector& CurrentEndLocation{ GetWeaponTraceSocketLocation() };

	TArray<FHitResult> HitResults{};
	FCollisionQueryParams Params{};
	Params.AddIgnoredActor(CachedOwnerEnemy);

	//@NOTE : bHit는 block일때만 true가 됨, overlap일때는 false
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		LastEndLocation,
		CurrentEndLocation,
		FQuat::Identity,
		ECC_WeaponHitCheck,
		FCollisionShape::MakeSphere(EnemyBasicAttackData.TraceRadius),
		Params
	);

#if 1
	DrawDebugLine(GetWorld(), LastEndLocation, CurrentEndLocation, FColor::Red, false, 2.0f);
	DrawDebugSphere(GetWorld(), LastEndLocation, EnemyBasicAttackData.TraceRadius, 8, FColor::Yellow, false, 2.0f);
	DrawDebugSphere(GetWorld(), CurrentEndLocation, EnemyBasicAttackData.TraceRadius, 8, FColor::Blue, false, 2.0f);
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
		}
	}

	for (const TPair<ANetherCrownCharacter*, FVector>& HitInfo : HitInfoMap)
	{
		Server_ReportHit(HitInfo.Key, HitInfo.Value);
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
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || CachedOwnerEnemy->HasAuthority())
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
	const ANetherCrownEnemyWeapon* HandledEnemyWeapon{ HandledEnemyWeaponWeak.Get() };
	const USkeletalMeshComponent* HandledEnemyWeaponMesh{ HandledEnemyWeapon ? HandledEnemyWeapon->GetEnemyWeaponSkeletalMeshComponent() : nullptr };
	if (!ensureAlways(IsValid(HandledEnemyWeaponMesh)))
	{
		return FVector::ZeroVector;
	}

	return HandledEnemyWeaponMesh->GetSocketLocation(EnemyBasicAttackData.WeaponTraceSocketName);
}

void UNetherCrownEnemyBasicAttackComponent::RequestEnemyAttack()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	if (EnemyBasicAttackState != ENetherCrownEnemyBasicAttackState::Attacking)
	{
		LastEndLocation = GetWeaponTraceSocketLocation();

		EnemyBasicAttackState = ENetherCrownEnemyBasicAttackState::Attacking;

		SetupBasicAttackTimer();

		Multicast_PlayBasicAttackMontage();
	}
}

void UNetherCrownEnemyBasicAttackComponent::Multicast_PlayBasicAttackMontage_Implementation()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || CachedOwnerEnemy->HasAuthority())
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

	if (!ensureAlways(IsValid(CachedBasicAttackMontage)))
	{
		return;
	}

	EnemyAnimInstance->Montage_Play(CachedBasicAttackMontage);
}
