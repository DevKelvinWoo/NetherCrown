// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownPortal.h"

#include "NetherCrown/GameMode/NetherCrownGameMode.h"

ANetherCrownPortal::ANetherCrownPortal()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	bNetLoadOnClient = true;
}

void ANetherCrownPortal::BeginPlay()
{
	Super::BeginPlay();
}

void ANetherCrownPortal::Interact()
{
	Super::Interact();

	if (HasAuthority())
	{
		TravelByLevelTag();
	}
}

void ANetherCrownPortal::TravelByLevelTag()
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
