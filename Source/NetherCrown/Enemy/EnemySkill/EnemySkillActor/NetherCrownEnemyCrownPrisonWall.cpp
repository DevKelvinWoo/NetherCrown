// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyCrownPrisonWall.h"

#include "NiagaraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

ANetherCrownEnemyCrownPrisonWall::ANetherCrownEnemyCrownPrisonWall()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	WallStaticMeshComponent1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallStaticMeshComponent1"));
	WallStaticMeshComponent1->SetupAttachment(RootComponent);
	WallStaticMeshComponents.Add(WallStaticMeshComponent1);

	WallStaticMeshComponent2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallStaticMeshComponent2"));
	WallStaticMeshComponent2->SetupAttachment(RootComponent);
	WallStaticMeshComponents.Add(WallStaticMeshComponent2);

	WallStaticMeshComponent3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallStaticMeshComponent3"));
	WallStaticMeshComponent3->SetupAttachment(RootComponent);
	WallStaticMeshComponents.Add(WallStaticMeshComponent3);

	WallStaticMeshComponent4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallStaticMeshComponent4"));
	WallStaticMeshComponent4->SetupAttachment(RootComponent);
	WallStaticMeshComponents.Add(WallStaticMeshComponent4);

	WallStaticMeshComponent5 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallStaticMeshComponent5"));
	WallStaticMeshComponent5->SetupAttachment(RootComponent);
	WallStaticMeshComponents.Add(WallStaticMeshComponent5);

	WallStaticMeshComponent6 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallStaticMeshComponent6"));
	WallStaticMeshComponent6->SetupAttachment(RootComponent);
	WallStaticMeshComponents.Add(WallStaticMeshComponent6);

	MagicRangeNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MagicRangeNiagaraComponent"));
	MagicRangeNiagaraComponent->SetupAttachment(RootComponent);
	MagicRangeNiagaraComponent->bAutoActivate = false;

	ExplosionNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ExplosionNiagaraComponent"));
	ExplosionNiagaraComponent->SetupAttachment(RootComponent);
	ExplosionNiagaraComponent->bAutoActivate = false;


	bReplicates = true;
	SetReplicatingMovement(true);
}

void ANetherCrownEnemyCrownPrisonWall::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);
}

void ANetherCrownEnemyCrownPrisonWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsRising || !HasAuthority())
	{
		return;
	}

	WallRiseElapsedTime += DeltaTime;

	const float Alpha{ FMath::Clamp(WallRiseElapsedTime / WallRiseDuration, 0.f, 1.f) };
	const float SmoothAlpha{ FMath::InterpEaseOut(0.f, 1.f, Alpha, 2.f) };
	SetActorLocation(FMath::Lerp(HiddenLocation, RaisedLocation, SmoothAlpha));

	if (Alpha >= 1.f)
	{
		HandleRiseWallFinished();
	}
}

void ANetherCrownEnemyCrownPrisonWall::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, WallHiddenZOffset);
}

void ANetherCrownEnemyCrownPrisonWall::StartRiseWall(const float InWallHiddenZOffset, const float InWallRiseDuration)
{
	if (!HasAuthority())
	{
		return;
	}

	WallHiddenZOffset = InWallHiddenZOffset;
	HiddenLocation = GetActorLocation();
	RaisedLocation = HiddenLocation - FVector{ 0.f, 0.f, InWallHiddenZOffset };
	WallRiseDuration = FMath::Max(InWallRiseDuration, KINDA_SMALL_NUMBER);
	WallRiseElapsedTime = 0.f;
	bIsRising = true;

	SetActorLocation(HiddenLocation);
	SetWallCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorTickEnabled(true);

	Multicast_ActiveWallRaiseUpDownNiagaraEffect(true);
}

void ANetherCrownEnemyCrownPrisonWall::SetWallCollisionEnabled(const ECollisionEnabled::Type CollisionEnabled) const
{
	if (!HasAuthority())
	{
		return;
	}

	for (UStaticMeshComponent* WallStaticMeshComponent : WallStaticMeshComponents)
	{
		if (!IsValid(WallStaticMeshComponent))
		{
			continue;
		}

		WallStaticMeshComponent->SetCollisionEnabled(CollisionEnabled);
	}
}

void ANetherCrownEnemyCrownPrisonWall::HandleRiseWallFinished()
{
	if (!HasAuthority())
	{
		return;
	}

	bIsRising = false;
	SetActorLocation(RaisedLocation);
	SetWallCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetActorTickEnabled(false);

	Multicast_ActiveMagicRangeNiagaraEffect();
}

void ANetherCrownEnemyCrownPrisonWall::Multicast_ActiveWallRaiseUpDownNiagaraEffect_Implementation(const bool bRaiseUp)
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	for (const UStaticMeshComponent* WallStaticMeshComponent : WallStaticMeshComponents)
	{
		if (!IsValid(WallStaticMeshComponent))
		{
			continue;
		}

		const FVector WallStaticMeshComponentLocation{ WallStaticMeshComponent->GetComponentLocation() };
		const FVector FinalLocation{ bRaiseUp ? WallStaticMeshComponentLocation - FVector{ 0.f, 0.f, WallHiddenZOffset } : WallStaticMeshComponentLocation };

		FNetherCrownUtilManager::SpawnNiagaraSystemByGameplayTag(this, NetherCrownTags::Effect_Boss1_CrownPrison_RaiseImpact, FTransform(FRotator::ZeroRotator, FinalLocation, FVector::OneVector));
	}
}

void ANetherCrownEnemyCrownPrisonWall::Multicast_ActiveMagicRangeNiagaraEffect_Implementation()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (!ensureAlways(IsValid(MagicRangeNiagaraComponent)))
	{
		return;
	}

	MagicRangeNiagaraComponent->Activate();
}
