// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownLevelTravelPersistenceSubsystem.h"

#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "UObject/UObjectGlobals.h"
#include "NetherCrown/UI/NetherCrownUIManagerSubsystem.h"

void UNetherCrownLevelTravelPersistenceSubsystem::BeginLevelTravelPersistence()
{
	if (bHasPendingPersistence)
	{
		RestoreLevelTravelPersistence();
		if (bHasPendingPersistence)
		{
			return;
		}
	}

	const UGameInstance* GameInstance{ GetGameInstance() };
	if (!ensureAlways(IsValid(GameInstance)))
	{
		return;
	}

	for (const ULocalPlayer* LocalPlayer : GameInstance->GetLocalPlayers())
	{
		if (!IsValid(LocalPlayer))
		{
			continue;
		}

		UNetherCrownUIManagerSubsystem* UIManagerSubsystem{ LocalPlayer->GetSubsystem<UNetherCrownUIManagerSubsystem>() };
		if (!IsValid(UIManagerSubsystem))
		{
			continue;
		}

		UIManagerSubsystem->CaptureTravelPersistentScreens();
		UIManagerSubsystem->PrepareForLevelTravel();
	}

	bHasPendingPersistence = true;
}

void UNetherCrownLevelTravelPersistenceSubsystem::RestoreLevelTravelPersistence()
{
	if (!bHasPendingPersistence)
	{
		return;
	}

	const UGameInstance* GameInstance{ GetGameInstance() };
	if (!ensureAlways(IsValid(GameInstance)))
	{
		return;
	}

	bool bHasRestoreTarget{ false };
	bool bIsRestoreComplete{ false };
	for (const ULocalPlayer* LocalPlayer : GameInstance->GetLocalPlayers())
	{
		if (!IsValid(LocalPlayer))
		{
			continue;
		}

		UNetherCrownUIManagerSubsystem* UIManagerSubsystem{ LocalPlayer->GetSubsystem<UNetherCrownUIManagerSubsystem>() };
		if (!IsValid(UIManagerSubsystem))
		{
			continue;
		}

		bHasRestoreTarget = true;
		bIsRestoreComplete &= UIManagerSubsystem->RestoreTravelPersistentScreens();
	}

	if (bHasRestoreTarget && bIsRestoreComplete)
	{
		bHasPendingPersistence = false;
	}
}
