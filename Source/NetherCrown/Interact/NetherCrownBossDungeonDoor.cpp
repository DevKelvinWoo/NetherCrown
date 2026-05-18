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

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	LeftDoorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoorMeshComponent"));
	LeftDoorMeshComponent->SetupAttachment(RootComponent);

	RightDoorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightDoorMeshComponent"));
	RightDoorMeshComponent->SetupAttachment(RootComponent);

	TestBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("TestBoxComponent"));
	TestBoxComponent->SetupAttachment(RootComponent);

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

	if (TestBoxComponent)
	{
		TestBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandleOnOverlapBegin);
	}
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

void ANetherCrownBossDungeonDoor::Interact()
{
	if (HasAuthority())
	{
		Multicast_PlayOpenDoorSound();
		Multicast_PlayOpenDoorCameraShake();
		Multicast_PlayOpenDoor();
	}
}

void ANetherCrownBossDungeonDoor::CacheBossDungeonDoorData()
{
	if (DoorDataAssetSoft.IsNull())
	{
		return;
	}

	CachedDoorDataAsset = DoorDataAssetSoft.LoadSynchronous();

	const FNetherCrownBossDungeonDoorData& DoorData{ CachedDoorDataAsset->GetBossDungeonDoorData() };
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

	if (!ensureAlways(IsValid(CameraShakePointSphereComponent)) || !ensureAlways(IsValid(CachedDoorDataAsset)))
	{
		return;
	}

	const FNetherCrownBossDungeonDoorData& DoorData{ CachedDoorDataAsset->GetBossDungeonDoorData() };
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

	if (!ensureAlways(IsValid(CachedDoorDataAsset)))
	{
		return;
	}

	const FNetherCrownBossDungeonDoorData& DoorData{ CachedDoorDataAsset->GetBossDungeonDoorData() };
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

void ANetherCrownBossDungeonDoor::HandleOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	Interact();
}
