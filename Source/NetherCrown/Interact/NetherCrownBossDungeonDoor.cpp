// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownBossDungeonDoor.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NetherCrown/Data/NetherCrownInteractData.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

ANetherCrownBossDungeonDoor::ANetherCrownBossDungeonDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	LeftDoorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoorMeshComponent"));
	LeftDoorMeshComponent->SetupAttachment(RootComponent);

	RightDoorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightDoorMeshComponent"));
	RightDoorMeshComponent->SetupAttachment(RootComponent);

	CameraShakePointSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CameraShakePointSphere"));
	CameraShakePointSphereComponent->SetupAttachment(RootComponent);

	bNetLoadOnClient = true;
	bReplicates = true;
}

void ANetherCrownBossDungeonDoor::BeginPlay()
{
	Super::BeginPlay();

	CacheBossDungeonDoorData();

	FOnTimelineFloat LeftDoorTimelineFloat{};
	LeftDoorTimelineFloat.BindUFunction(this, FName("OpenLeftDoor"));
	LeftDoorOpenTimeline.AddInterpFloat(CachedLeftDoorOpenCurve, LeftDoorTimelineFloat);

	FOnTimelineFloat RightDoorTimelineFloat{};
	RightDoorTimelineFloat.BindUFunction(this, FName("OpenRightDoor"));
	RightDoorOpenTimeline.AddInterpFloat(CachedRightDoorOpenCurve, RightDoorTimelineFloat);
}

void ANetherCrownBossDungeonDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (LeftDoorOpenTimeline.IsPlaying())
	{
		LeftDoorOpenTimeline.TickTimeline(DeltaTime);
	}

	if (RightDoorOpenTimeline.IsPlaying())
	{
		RightDoorOpenTimeline.TickTimeline(DeltaTime);
	}
}

void ANetherCrownBossDungeonDoor::FinishInteract(ANetherCrownCharacter* InteractCharacter)
{
	Super::FinishInteract(InteractCharacter);

	if (HasAuthority())
	{
		Multicast_PlayOpenDoorSound();
		Multicast_PlayOpenDoorCameraShake();
		Multicast_PlayOpenDoor();
	}
}

void ANetherCrownBossDungeonDoor::CacheBossDungeonDoorData()
{
	const UNetherCrownBossDungeonDoorDataAsset* DoorDataAsset{ Cast<UNetherCrownBossDungeonDoorDataAsset>(GetCachedInteractDataAsset()) };
	if (!ensureAlways(IsValid(DoorDataAsset)))
	{
		return;
	}

	const FNetherCrownBossDungeonDoorData& DoorData{ DoorDataAsset->GetBossDungeonDoorData() };
	if (!DoorData.LeftDoorOpenCurve.IsNull())
	{
		CachedLeftDoorOpenCurve = DoorData.LeftDoorOpenCurve.LoadSynchronous();
	}

	if (!DoorData.RightDoorOpenCurve.IsNull())
	{
		CachedRightDoorOpenCurve = DoorData.RightDoorOpenCurve.LoadSynchronous();
	}
}

void ANetherCrownBossDungeonDoor::Multicast_PlayOpenDoor_Implementation()
{
	LeftDoorOpenTimeline.PlayFromStart();
	RightDoorOpenTimeline.PlayFromStart();
}

void ANetherCrownBossDungeonDoor::Multicast_PlayOpenDoorCameraShake_Implementation()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	const UNetherCrownBossDungeonDoorDataAsset* DoorDataAsset{ Cast<UNetherCrownBossDungeonDoorDataAsset>(GetCachedInteractDataAsset()) };
	if (!ensureAlways(IsValid(CameraShakePointSphereComponent)) || !ensureAlways(IsValid(DoorDataAsset)))
	{
		return;
	}

	const FNetherCrownBossDungeonDoorData& DoorData{ DoorDataAsset->GetBossDungeonDoorData() };
	UGameplayStatics::PlayWorldCameraShake(
		this,
		DoorData.OpenDoorCameraShakeClass,
		CameraShakePointSphereComponent->GetComponentLocation(),
		DoorData.OpenDoorCameraShakeInnerRadius,
		DoorData.OpenDoorCameraShakeOuterRadius
		);
}

void ANetherCrownBossDungeonDoor::Multicast_PlayOpenDoorSound_Implementation()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	const UNetherCrownBossDungeonDoorDataAsset* DoorDataAsset{ Cast<UNetherCrownBossDungeonDoorDataAsset>(GetCachedInteractDataAsset()) };
	if (!ensureAlways(IsValid(DoorDataAsset)))
	{
		return;
	}

	const FNetherCrownBossDungeonDoorData& DoorData{ DoorDataAsset->GetBossDungeonDoorData() };
	FNetherCrownUtilManager::PlaySound2DByGameplayTag(this, DoorData.BossDungeonDoorTagData.OpenDoorSoundTag);
}

void ANetherCrownBossDungeonDoor::OpenLeftDoor(float CurveFloat)
{
	LeftDoorMeshComponent->SetRelativeLocation(FVector(0.f, CurveFloat, 0.f));
}

void ANetherCrownBossDungeonDoor::OpenRightDoor(float CurveFloat)
{
	RightDoorMeshComponent->SetRelativeLocation(FVector(0.f, CurveFloat, 0.f));
}
