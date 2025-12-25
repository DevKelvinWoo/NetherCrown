// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownWeaponTraceComponent.h"

#include "NetherCrownWeapon.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Settings/NetherCrownCharacterDefaultSettings.h"
#include "NetherCrown/Util/NetherCrownCollisionChannels.h"

#define TRACE_DEBUG 1

UNetherCrownWeaponTraceComponent::UNetherCrownWeaponTraceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void UNetherCrownWeaponTraceComponent::InitWeaponTraceComponentSettings(const FVector& InLastEndLocation)
{
	LastEndLocation = InLastEndLocation;
	HitIgnoreEnemies.Empty();
}

void UNetherCrownWeaponTraceComponent::BeginPlay()
{
	Super::BeginPlay();

	const UNetherCrownCharacterDefaultSettings* CharacterDefaultSettings{ GetDefault<UNetherCrownCharacterDefaultSettings>() };
	check(CharacterDefaultSettings);

	WeaponTraceSocketName = CharacterDefaultSettings->WeaponTraceSocketName;

	const ANetherCrownWeapon* OwnerWeapon{ Cast<ANetherCrownWeapon>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerWeapon)))
	{
		return;
	}

	WeaponMesh = OwnerWeapon->GetWeaponMeshComponent();
}

void UNetherCrownWeaponTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsTraceEnabled)
	{
		DetectWeaponHit();
	}
}

void UNetherCrownWeaponTraceComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsTraceEnabled);
	DOREPLIFETIME(ThisClass, LastEndLocation);
	DOREPLIFETIME(ThisClass, HitIgnoreEnemies);
}

void UNetherCrownWeaponTraceComponent::DetectWeaponHit()
{
	//@NOTE : Server에서만 실행시 Animation Render를 따라가지 못해 정확한 Trace지점을 찾지 못함 따라서 client에서도 호출함
	if (!ensureAlways(IsValid(WeaponMesh)))
	{
		return;
	}

	AActor* Owner{ GetOwner() };
	if (!Owner || Owner->HasAuthority())
	{
		return;
	}

	const FVector& CurrentEndLocation{ WeaponMesh->GetSocketLocation(WeaponTraceSocketName) };

	TArray<FHitResult> HitResults{};
	FCollisionQueryParams Params{};
	Params.AddIgnoredActor(Owner);

	for (const ANetherCrownEnemy* HitEnemy : HitIgnoreEnemies)
	{
		Params.AddIgnoredActor(HitEnemy);
	}

	//@NOTE : bHit는 block일때만 true가 됨, overlap일때는 false
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		LastEndLocation,
		CurrentEndLocation,
		FQuat::Identity,
		ECC_WeaponHitCheck,
		FCollisionShape::MakeSphere(TraceRadius),
		Params
	);

#if TRACE_DEBUG
	DrawDebugLine(GetWorld(), LastEndLocation, CurrentEndLocation, FColor::Red, false, 2.0f);
	DrawDebugSphere(GetWorld(), LastEndLocation, TraceRadius, 8, FColor::Yellow, false, 2.0f);
	DrawDebugSphere(GetWorld(), CurrentEndLocation, TraceRadius, 8, FColor::Blue, false, 2.0f);
#endif

	LastEndLocation = CurrentEndLocation;

	if (HitResults.IsEmpty())
	{
		return;
	}

	TMap<ANetherCrownEnemy*, FVector> HitInfoMap{};
	for(const FHitResult& Hit : HitResults)
	{
		if (ANetherCrownEnemy* HitEnemy = Cast<ANetherCrownEnemy>(Hit.GetActor()))
		{
			HitInfoMap.Add(HitEnemy, Hit.ImpactPoint);
			HitIgnoreEnemies.AddUnique(HitEnemy);
		}
	}

	for (const TPair<ANetherCrownEnemy*, FVector>& HitInfo : HitInfoMap)
	{
		//@NOTE : Server로 HitEnemy Damage를 줘야함, 위 공격 감지로직은 Client에서 정확하게 실행됨
		//@NOTE : ServerRPC로 UGameplayStatics::ApplyDamage()를 줘야함, 서버에서 ApplyDamage가 호출되면 TakeDamage도 서버에서 수행됨

		OnHitEnemy.Broadcast(HitInfo.Key, HitInfo.Value);
	}
}
