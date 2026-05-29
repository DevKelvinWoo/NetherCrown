// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownLevelTravelPersistenceSubsystem.h"

#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "UObject/UObjectGlobals.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Character/NetherCrownPlayerController.h"
#include "NetherCrown/Components/NetherCrownEquipComponent.h"
#include "NetherCrown/Components/NetherCrownQuestComponent.h"
#include "NetherCrown/PlayerState/NetherCrownPlayerState.h"
#include "NetherCrown/Settings/NetherCrownDefaultSettings.h"
#include "NetherCrown/UI/NetherCrownUIManagerSubsystem.h"
#include "NetherCrown/Widgets/NetherCrownLoadingScreenWidgetView.h"

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

	CaptureWeaponPersistentData();
	CaptureQuestPersistentData();

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

	ShowLoadingScreen();
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
	bool bIsRestoreComplete{ true };
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

	const bool bHadWeaponRestoreTarget{ !WeaponPersistentDataMap.IsEmpty() };
	const bool bIsWeaponRestoreComplete{ RestoreWeaponPersistentData() };
	bHasRestoreTarget |= bHadWeaponRestoreTarget;
	bIsRestoreComplete &= bIsWeaponRestoreComplete;

	const bool bHadQuestRestoreTarget{ !QuestPersistentDataMap.IsEmpty() };
	const bool bIsQuestRestoreComplete{ RestoreQuestPersistentData() };
	bHasRestoreTarget |= bHadQuestRestoreTarget;
	bIsRestoreComplete &= bIsQuestRestoreComplete;

	if (bHasRestoreTarget && bIsRestoreComplete)
	{
		HideLoadingScreen();
		bHasPendingPersistence = false;
	}
}

void UNetherCrownLevelTravelPersistenceSubsystem::CaptureWeaponPersistentData()
{
	WeaponPersistentDataMap.Empty();
	WeaponPersistentPlayerIdQueue.Empty();

	UWorld* World{ GetWorld() };
	if (!IsValid(World) || !World->GetAuthGameMode())
	{
		return;
	}

	for (FConstPlayerControllerIterator PlayerControllerIterator{ World->GetPlayerControllerIterator() }; PlayerControllerIterator; ++PlayerControllerIterator)
	{
		const ANetherCrownPlayerController* PlayerController{ Cast<ANetherCrownPlayerController>(PlayerControllerIterator->Get()) };
		if (!IsValid(PlayerController))
		{
			continue;
		}

		const ANetherCrownPlayerState* PlayerState{ PlayerController->GetPlayerState<ANetherCrownPlayerState>() };
		if (!IsValid(PlayerState))
		{
			continue;
		}

		const FGuid& PersistentPlayerId{ PlayerState->GetPersistentPlayerId() };
		if (!PersistentPlayerId.IsValid())
		{
			continue;
		}

		ANetherCrownCharacter* ControlledCharacter{ PlayerController->GetCachedCharacter() };
		if (!IsValid(ControlledCharacter))
		{
			ControlledCharacter = Cast<ANetherCrownCharacter>(PlayerController->GetPawn());
		}

		if (!IsValid(ControlledCharacter))
		{
			WeaponPersistentDataMap.Add(PersistentPlayerId, PlayerState->GetWeaponPersistentData());
			WeaponPersistentPlayerIdQueue.Add(PersistentPlayerId);
			continue;
		}

		const UNetherCrownEquipComponent* EquipComponent{ ControlledCharacter->GetEquipComponent() };
		if (!IsValid(EquipComponent))
		{
			WeaponPersistentDataMap.Add(PersistentPlayerId, PlayerState->GetWeaponPersistentData());
			WeaponPersistentPlayerIdQueue.Add(PersistentPlayerId);
			continue;
		}

		const FNetherCrownWeaponPersistentData WeaponPersistentData{ EquipComponent->MakeWeaponPersistentData() };
		WeaponPersistentDataMap.Add(PersistentPlayerId, WeaponPersistentData);
		WeaponPersistentPlayerIdQueue.Add(PersistentPlayerId);
	}
}

bool UNetherCrownLevelTravelPersistenceSubsystem::RestoreWeaponPersistentData()
{
	if (WeaponPersistentDataMap.IsEmpty())
	{
		return true;
	}

	UWorld* World{ GetWorld() };
	if (!IsValid(World) || !World->GetAuthGameMode())
	{
		return true;
	}

	bool bIsRestoreComplete{ true };
	TArray<FGuid> RestoredPlayerIds{};

	for (FConstPlayerControllerIterator PlayerControllerIterator{ World->GetPlayerControllerIterator() }; PlayerControllerIterator; ++PlayerControllerIterator)
	{
		ANetherCrownPlayerController* PlayerController{ Cast<ANetherCrownPlayerController>(PlayerControllerIterator->Get()) };
		if (!IsValid(PlayerController))
		{
			continue;
		}

		ANetherCrownPlayerState* PlayerState{ PlayerController->GetPlayerState<ANetherCrownPlayerState>() };
		if (!IsValid(PlayerState))
		{
			bIsRestoreComplete = false;
			continue;
		}

		FGuid PersistentPlayerId{ PlayerState->GetPersistentPlayerId() };
		if (!PersistentPlayerId.IsValid())
		{
			PersistentPlayerId = PlayerController->GetPersistentPlayerId();
		}

		if (!PersistentPlayerId.IsValid())
		{
			for (const FGuid& CandidatePersistentPlayerId : WeaponPersistentPlayerIdQueue)
			{
				if (WeaponPersistentDataMap.Contains(CandidatePersistentPlayerId))
				{
					PersistentPlayerId = CandidatePersistentPlayerId;
					PlayerController->SetPersistentPlayerId(PersistentPlayerId);
					PlayerState->SetPersistentPlayerId(PersistentPlayerId);
					break;
				}
			}
		}

		if (!PersistentPlayerId.IsValid())
		{
			bIsRestoreComplete = false;
			continue;
		}

		const FNetherCrownWeaponPersistentData* WeaponPersistentData{ WeaponPersistentDataMap.Find(PersistentPlayerId) };
		if (!WeaponPersistentData)
		{
			continue;
		}

		ANetherCrownCharacter* ControlledCharacter{ PlayerController->GetCachedCharacter() };
		if (!IsValid(ControlledCharacter))
		{
			ControlledCharacter = Cast<ANetherCrownCharacter>(PlayerController->GetPawn());
		}

		if (!IsValid(ControlledCharacter))
		{
			bIsRestoreComplete = false;
			continue;
		}

		UNetherCrownEquipComponent* EquipComponent{ ControlledCharacter->GetEquipComponent() };
		if (!IsValid(EquipComponent))
		{
			bIsRestoreComplete = false;
			continue;
		}

		if (EquipComponent->RestoreWeaponFromPersistentData(*WeaponPersistentData))
		{
			RestoredPlayerIds.Add(PersistentPlayerId);
		}
		else
		{
			bIsRestoreComplete = false;
		}
	}

	for (const FGuid& RestoredPlayerId : RestoredPlayerIds)
	{
		WeaponPersistentDataMap.Remove(RestoredPlayerId);
		WeaponPersistentPlayerIdQueue.Remove(RestoredPlayerId);
	}

	return bIsRestoreComplete && WeaponPersistentDataMap.IsEmpty();
}

void UNetherCrownLevelTravelPersistenceSubsystem::CaptureQuestPersistentData()
{
	QuestPersistentDataMap.Empty();
	QuestPersistentPlayerIdQueue.Empty();

	UWorld* World{ GetWorld() };
	if (!IsValid(World) || !World->GetAuthGameMode())
	{
		return;
	}

	for (FConstPlayerControllerIterator PlayerControllerIterator{ World->GetPlayerControllerIterator() }; PlayerControllerIterator; ++PlayerControllerIterator)
	{
		const ANetherCrownPlayerController* PlayerController{ Cast<ANetherCrownPlayerController>(PlayerControllerIterator->Get()) };
		if (!IsValid(PlayerController))
		{
			continue;
		}

		const ANetherCrownPlayerState* PlayerState{ PlayerController->GetPlayerState<ANetherCrownPlayerState>() };
		if (!IsValid(PlayerState))
		{
			continue;
		}

		const FGuid& PersistentPlayerId{ PlayerState->GetPersistentPlayerId() };
		if (!PersistentPlayerId.IsValid())
		{
			continue;
		}

		ANetherCrownCharacter* ControlledCharacter{ PlayerController->GetCachedCharacter() };
		if (!IsValid(ControlledCharacter))
		{
			ControlledCharacter = Cast<ANetherCrownCharacter>(PlayerController->GetPawn());
		}

		if (!IsValid(ControlledCharacter))
		{
			QuestPersistentDataMap.Add(PersistentPlayerId, PlayerState->GetQuestPersistentData());
			QuestPersistentPlayerIdQueue.Add(PersistentPlayerId);
			continue;
		}

		const UNetherCrownQuestComponent* QuestComponent{ ControlledCharacter->GetQuestComponent() };
		if (!IsValid(QuestComponent))
		{
			QuestPersistentDataMap.Add(PersistentPlayerId, PlayerState->GetQuestPersistentData());
			QuestPersistentPlayerIdQueue.Add(PersistentPlayerId);
			continue;
		}

		const FNetherCrownQuestPersistentData QuestPersistentData{ QuestComponent->MakeQuestPersistentData() };
		QuestPersistentDataMap.Add(PersistentPlayerId, QuestPersistentData);
		QuestPersistentPlayerIdQueue.Add(PersistentPlayerId);
	}
}

bool UNetherCrownLevelTravelPersistenceSubsystem::RestoreQuestPersistentData()
{
	if (QuestPersistentDataMap.IsEmpty())
	{
		return true;
	}

	UWorld* World{ GetWorld() };
	if (!IsValid(World) || !World->GetAuthGameMode())
	{
		return true;
	}

	bool bIsRestoreComplete{ true };
	TArray<FGuid> RestoredPlayerIds{};

	for (FConstPlayerControllerIterator PlayerControllerIterator{ World->GetPlayerControllerIterator() }; PlayerControllerIterator; ++PlayerControllerIterator)
	{
		ANetherCrownPlayerController* PlayerController{ Cast<ANetherCrownPlayerController>(PlayerControllerIterator->Get()) };
		if (!IsValid(PlayerController))
		{
			continue;
		}

		ANetherCrownPlayerState* PlayerState{ PlayerController->GetPlayerState<ANetherCrownPlayerState>() };
		if (!IsValid(PlayerState))
		{
			bIsRestoreComplete = false;
			continue;
		}

		FGuid PersistentPlayerId{ PlayerState->GetPersistentPlayerId() };
		if (!PersistentPlayerId.IsValid())
		{
			PersistentPlayerId = PlayerController->GetPersistentPlayerId();
		}

		if (!PersistentPlayerId.IsValid())
		{
			for (const FGuid& CandidatePersistentPlayerId : QuestPersistentPlayerIdQueue)
			{
				if (QuestPersistentDataMap.Contains(CandidatePersistentPlayerId))
				{
					PersistentPlayerId = CandidatePersistentPlayerId;
					PlayerController->SetPersistentPlayerId(PersistentPlayerId);
					PlayerState->SetPersistentPlayerId(PersistentPlayerId);
					break;
				}
			}
		}

		if (!PersistentPlayerId.IsValid())
		{
			bIsRestoreComplete = false;
			continue;
		}

		const FNetherCrownQuestPersistentData* QuestPersistentData{ QuestPersistentDataMap.Find(PersistentPlayerId) };
		if (!QuestPersistentData)
		{
			continue;
		}

		ANetherCrownCharacter* ControlledCharacter{ PlayerController->GetCachedCharacter() };
		if (!IsValid(ControlledCharacter))
		{
			ControlledCharacter = Cast<ANetherCrownCharacter>(PlayerController->GetPawn());
		}

		if (!IsValid(ControlledCharacter))
		{
			bIsRestoreComplete = false;
			continue;
		}

		UNetherCrownQuestComponent* QuestComponent{ ControlledCharacter->GetQuestComponent() };
		if (!IsValid(QuestComponent))
		{
			bIsRestoreComplete = false;
			continue;
		}

		if (QuestComponent->RestoreQuestFromPersistentData(*QuestPersistentData))
		{
			RestoredPlayerIds.Add(PersistentPlayerId);
		}
		else
		{
			bIsRestoreComplete = false;
		}
	}

	for (const FGuid& RestoredPlayerId : RestoredPlayerIds)
	{
		QuestPersistentDataMap.Remove(RestoredPlayerId);
		QuestPersistentPlayerIdQueue.Remove(RestoredPlayerId);
	}

	return bIsRestoreComplete && QuestPersistentDataMap.IsEmpty();
}

void UNetherCrownLevelTravelPersistenceSubsystem::ShowLoadingScreen()
{
	if (!LoadingScreenWidgets.IsEmpty())
	{
		return;
	}

	const UNetherCrownDefaultSettings* DefaultSettings{ GetDefault<UNetherCrownDefaultSettings>() };
	check(DefaultSettings);

	if (!DefaultSettings->GetLoadingScreenWidgetClass())
	{
		return;
	}

	UGameInstance* GameInstance{ GetGameInstance() };
	if (!ensureAlways(IsValid(GameInstance)))
	{
		return;
	}

	for (ULocalPlayer* LocalPlayer : GameInstance->GetLocalPlayers())
	{
		if (!IsValid(LocalPlayer))
		{
			continue;
		}

		APlayerController* PlayerController{ LocalPlayer->GetPlayerController(GetWorld()) };
		if (!IsValid(PlayerController))
		{
			continue;
		}

		UNetherCrownLoadingScreenWidgetView* LoadingScreenWidget{ CreateWidget<UNetherCrownLoadingScreenWidgetView>(PlayerController, DefaultSettings->GetLoadingScreenWidgetClass()) };
		if (!ensureAlways(IsValid(LoadingScreenWidget)))
		{
			continue;
		}

		LoadingScreenWidget->AddToViewport();
		LoadingScreenWidgets.Add(LoadingScreenWidget);
	}
}

void UNetherCrownLevelTravelPersistenceSubsystem::HideLoadingScreen()
{
	for (UNetherCrownLoadingScreenWidgetView* LoadingScreenWidget : LoadingScreenWidgets)
	{
		if (IsValid(LoadingScreenWidget))
		{
			LoadingScreenWidget->RemoveFromParent();
		}
	}

	LoadingScreenWidgets.Empty();
}
