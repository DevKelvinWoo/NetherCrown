// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyCrownPrisonWall.h"

#include "NiagaraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/Util/NetherCrownCollisionChannels.h"
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

void ANetherCrownEnemyCrownPrisonWall::InitCrownPrisonWall(const FNetherCrownCrownPrisonInitData& InCrownPrisonInitData)
{
	if (!HasAuthority())
	{
		return;
	}

	CrownPrisonInitData = InCrownPrisonInitData;
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
	DOREPLIFETIME(ThisClass, WorldCameraShakeLocation);
}

void ANetherCrownEnemyCrownPrisonWall::StartRiseUpOrDownWall(const float InWallHiddenZOffset, const float InWallRiseDuration, const bool bRiseUp)
{
	if (!HasAuthority())
	{
		return;
	}

	Multicast_PlayCrownPrisonSoundByTag(NetherCrownTags::Sound_BossEnemy1_CrownPrisonStatueRise);

	WallHiddenZOffset = InWallHiddenZOffset;
	HiddenLocation = GetActorLocation();
	RaisedLocation = bRiseUp ? HiddenLocation - FVector{ 0.f, 0.f, InWallHiddenZOffset } : HiddenLocation + FVector{ 0.f, 0.f, InWallHiddenZOffset };
	WorldCameraShakeLocation = bRiseUp ? RaisedLocation : HiddenLocation;
	WallRiseDuration = FMath::Max(InWallRiseDuration, KINDA_SMALL_NUMBER);
	WallRiseElapsedTime = 0.f;
	bIsRising = true;

	bIsRiseUp = bRiseUp;

	SetActorLocation(HiddenLocation);
	SetWallCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorTickEnabled(true);

	Multicast_ActiveWallRaiseUpDownNiagaraEffect(bRiseUp);
	Multicast_PlayRiseWallCameraShake();
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

	if (!bIsRiseUp)
	{
		StartDestroyTimer();
		return;
	}

	bIsRising = false;
	SetActorLocation(RaisedLocation);
	SetWallCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetActorTickEnabled(false);

	WorldCameraShakeLocation = GetActorLocation();

	Multicast_ActiveMagicRangeNiagaraEffect();
	Multicast_PlayCrownPrisonSoundByTag(NetherCrownTags::Sound_BossEnemy1_CrownPrisonRanged);
	StartExplosionTimer();
}

void ANetherCrownEnemyCrownPrisonWall::StartExplosionTimer()
{
	if (!HasAuthority())
	{
		return;
	}

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	FTimerManager& TimerManager{ World->GetTimerManager() };
	TimerManager.ClearTimer(ExplosionTimer);
	TimerManager.SetTimer(ExplosionTimer, this, &ThisClass::ExplosionPrisonWallMagic, CrownPrisonInitData.ExplosionTimeOffset, false);
}

void ANetherCrownEnemyCrownPrisonWall::ExplosionPrisonWallMagic()
{
	if (!HasAuthority())
	{
		return;
	}

	StartRiseDownWallTimer();

	Multicast_ActiveExplosionNiagaraEffect();
	Multicast_PlayCrownPrisonSoundByTag(NetherCrownTags::Sound_BossEnemy1_CrownPrisonExplosion);

	TArray<AActor*> OverlappedActors{};
	const FVector DetectSpherePos{ GetActorLocation() };
	const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes{ UEngineTypes::ConvertToObjectType(ECC_Player) };

	UKismetSystemLibrary::SphereOverlapActors(this, DetectSpherePos, CrownPrisonInitData.ExplosionRadius, ObjectTypes, ANetherCrownCharacter::StaticClass(),
		TArray<AActor*>(), OverlappedActors);

	if (OverlappedActors.IsEmpty())
	{
		return;
	}

	for (AActor* OverlappedActor : OverlappedActors)
	{
		if (ANetherCrownCharacter* OverlappedCharacter = Cast<ANetherCrownCharacter>(OverlappedActor))
		{
			OnCrownPrisonExplosionHit.Broadcast(OverlappedCharacter);
		}
	}
}

void ANetherCrownEnemyCrownPrisonWall::StartRiseDownWallTimer()
{
	if (!HasAuthority())
	{
		return;
	}

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	FTimerManager& TimerManager{ World->GetTimerManager() };
	TimerManager.ClearTimer(RiseDownWallTimer);
	TimerManager.SetTimer(RiseDownWallTimer, this, &ThisClass::RiseDownWall, 1.5f, false);
}

void ANetherCrownEnemyCrownPrisonWall::RiseDownWall()
{
	if (!HasAuthority())
	{
		return;
	}

	StartRiseUpOrDownWall(WallHiddenZOffset, WallRiseDuration, false);
}

void ANetherCrownEnemyCrownPrisonWall::StartDestroyTimer()
{
	if (!HasAuthority())
	{
		return;
	}
	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	FTimerDelegate TimerDelegate{};
	TimerDelegate.BindLambda([WeakThis = MakeWeakObjectPtr(this)]()
	{
		ANetherCrownEnemyCrownPrisonWall* ThisPtr{ WeakThis.Get() };
		if (!ensureAlways(IsValid(ThisPtr)))
		{
			return;
		}

		ThisPtr->Destroy();
	});

	FTimerManager& TimerManager{ World->GetTimerManager() };
	TimerManager.ClearTimer(DestroyTimer);
	TimerManager.SetTimer(DestroyTimer, TimerDelegate, 1.5f, false);
}

void ANetherCrownEnemyCrownPrisonWall::Multicast_PlayCrownPrisonSoundByTag_Implementation(const FGameplayTag& SoundTag)
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	FNetherCrownUtilManager::PlaySound2DByGameplayTag(this, SoundTag);
}

void ANetherCrownEnemyCrownPrisonWall::Multicast_ActiveExplosionNiagaraEffect_Implementation()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (!ensureAlways(IsValid(ExplosionNiagaraComponent)))
	{
		return;
	}

	ExplosionNiagaraComponent->Activate();
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

void ANetherCrownEnemyCrownPrisonWall::Multicast_PlayRiseWallCameraShake_Implementation()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (!IsValid(RiseWallCameraShakeClass))
	{
		return;
	}

	UGameplayStatics::PlayWorldCameraShake(
		this,
		RiseWallCameraShakeClass,
		WorldCameraShakeLocation,
		RiseWallCameraShakeInnerRadius,
		RiseWallCameraShakeOuterRadius,
		RiseWallCameraShakeFalloff,
		false
	);
}
