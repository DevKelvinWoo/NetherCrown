// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownWeaponTraceComponent.h"

#include "DrawDebugHelpers.h"
#include "NetherCrownWeapon.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Util/NetherCrownCollisionChannels.h"

#define TRACE_DEBUG 0

UNetherCrownWeaponTraceComponent::UNetherCrownWeaponTraceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void UNetherCrownWeaponTraceComponent::InitWeaponTraceComponentSettings(const FVector& InLastEndLocation)
{
	CacheWeaponTraceData();
	LastEndLocation = InLastEndLocation;
	HitIgnoreEnemies.Empty();
}

void UNetherCrownWeaponTraceComponent::BeginPlay()
{
	Super::BeginPlay();

	const ANetherCrownWeapon* OwnerWeapon{ Cast<ANetherCrownWeapon>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerWeapon)))
	{
		return;
	}

	WeaponMesh = OwnerWeapon->GetWeaponMeshComponent();
}

void UNetherCrownWeaponTraceComponent::CacheWeaponTraceData()
{
	const ANetherCrownWeapon* OwnerWeapon{ Cast<ANetherCrownWeapon>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerWeapon)))
	{
		return;
	}

	const UNetherCrownWeaponData* OwnerWeaponData{ OwnerWeapon->GetWeaponData() };
	if (!ensureAlways(IsValid(OwnerWeaponData)))
	{
		return;
	}

	WeaponTraceData = OwnerWeaponData->GetWeaponTraceData();
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
	DOREPLIFETIME(ThisClass, TraceMode);
}

void UNetherCrownWeaponTraceComponent::DetectWeaponHit()
{
	switch (TraceMode)
	{
	case ENetherCrownTraceMode::Swing:
		DetectWeaponSwingHit();
		break;
	case ENetherCrownTraceMode::Thrust:
		DetectWeaponThrustHit();
		break;
	}
}

void UNetherCrownWeaponTraceComponent::DetectWeaponSwingHit()
{
	//@NOTE : Server에서만 실행시 Animation Render를 따라가지 못해 정확한 Trace지점을 찾지 못함 따라서 client에서도 호출함
	if (!ensureAlways(IsValid(WeaponMesh)))
	{
		return;
	}

	AActor* Owner{ GetOwner() };
	if (!ensureAlways(IsValid(Owner)) || Owner->HasAuthority())
	{
		return;
	}

	const FVector& CurrentEndLocation{ WeaponMesh->GetSocketLocation(WeaponTraceData.WeaponTraceSocketName) };

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
		FCollisionShape::MakeSphere(WeaponTraceData.SwingTraceRadius),
		Params
	);

#if TRACE_DEBUG
	DrawDebugLine(GetWorld(), LastEndLocation, CurrentEndLocation, FColor::Red, false, 2.0f);
	DrawDebugSphere(GetWorld(), LastEndLocation, WeaponTraceData.SwingTraceRadius, 8, FColor::Yellow, false, 2.0f);
	DrawDebugSphere(GetWorld(), CurrentEndLocation, WeaponTraceData.SwingTraceRadius, 8, FColor::Blue, false, 2.0f);
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
		Server_ReportHit(HitInfo.Key, HitInfo.Value);
	}
}

void UNetherCrownWeaponTraceComponent::DetectWeaponThrustHit()
{
	if (!ensureAlways(IsValid(GetOwner())))
	{
		return;
	}

	AActor* Owner{ GetOwner() };
	if (!ensureAlways(IsValid(Owner)) || Owner->HasAuthority())
	{
		return;
	}

	const ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(Owner->GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)))
	{
		return;
	}

	const FVector ForwardVector{ OwnerCharacter->GetActorForwardVector() };
	const FVector StartLocation
	{
		OwnerCharacter->GetActorLocation() + ForwardVector * WeaponTraceData.ThrustStartForwardOffset + FVector(0.f, 0.f, WeaponTraceData.ThrustStartUpOffset)
	};
	const FVector EndLocation{ StartLocation + ForwardVector * WeaponTraceData.ThrustTraceLength };

	TArray<FHitResult> HitResults{};
	FCollisionQueryParams Params{};
	Params.AddIgnoredActor(Owner);

	for (const ANetherCrownEnemy* HitEnemy : HitIgnoreEnemies)
	{
		Params.AddIgnoredActor(HitEnemy);
	}

	GetWorld()->SweepMultiByChannel(
			HitResults,
			StartLocation,
			EndLocation,
			FQuat::Identity,
			ECC_WeaponHitCheck,
			FCollisionShape::MakeSphere(WeaponTraceData.ThrustTraceRadius),
			Params
	);

#if 0
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Cyan, false, 2.0f);
	DrawDebugSphere(GetWorld(), StartLocation, WeaponTraceData.ThrustTraceRadius, 12, FColor::Yellow, false, 2.0f);
	DrawDebugSphere(GetWorld(), EndLocation, WeaponTraceData.ThrustTraceRadius, 12, FColor::Blue, false, 2.0f);

	const FVector MidPoint{ (StartLocation + EndLocation) * 0.5f };
	const double HalfHeight{ FVector::Distance(StartLocation, EndLocation) * 0.5f + WeaponTraceData.ThrustTraceRadius };
	DrawDebugCapsule(GetWorld(), MidPoint, HalfHeight, WeaponTraceData.ThrustTraceRadius, FRotationMatrix::MakeFromX(EndLocation - StartLocation).ToQuat(), FColor::Green, false, 2.0f);
#endif

	if (HitResults.IsEmpty())
	{
		return;
	}

	TMap<ANetherCrownEnemy*, FVector> HitInfoMap{};
	for (const FHitResult& Hit : HitResults)
	{
		if (ANetherCrownEnemy* HitEnemy = Cast<ANetherCrownEnemy>(Hit.GetActor()))
		{
			HitInfoMap.Add(HitEnemy, Hit.ImpactPoint);
			HitIgnoreEnemies.AddUnique(HitEnemy);
		}
	}

	for (const TPair<ANetherCrownEnemy*, FVector>& HitInfo : HitInfoMap)
	{
		Server_ReportHit(HitInfo.Key, HitInfo.Value);
	}
}

void UNetherCrownWeaponTraceComponent::Server_ReportHit_Implementation(ANetherCrownEnemy* HitEnemy, const FVector& HitLocation)
{
	if (IsValid(HitEnemy))
	{
		OnHitEnemy.Broadcast(HitEnemy, HitLocation);
	}
}
