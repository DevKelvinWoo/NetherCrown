// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownPortal.h"

#include "Components/BoxComponent.h"
#include "NetherCrown/GameMode/NetherCrownGameMode.h"

ANetherCrownPortal::ANetherCrownPortal()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	TestBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("TestBoxComponent"));
	TestBoxComponent->SetupAttachment(RootComponent);

	bReplicates = true;
	bNetLoadOnClient = true;
}

void ANetherCrownPortal::BeginPlay()
{
	Super::BeginPlay();

	if (ensureAlways(IsValid(TestBoxComponent)))
	{
		TestBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnTestBoxBeginOverlap);
	}
}

void ANetherCrownPortal::OnTestBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

	ANetherCrownGameMode* NetherCrownGameMode{ World->GetAuthGameMode<ANetherCrownGameMode>() };
	if (!ensureAlways(IsValid(NetherCrownGameMode)))
	{
		return;
	}

	NetherCrownGameMode->TravelLevelByTag(LevelTag);
}
