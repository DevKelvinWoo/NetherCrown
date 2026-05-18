// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownGameMode.h"

#include "NetherCrown/NetherCrown.h"
#include "NetherCrown/Character/NetherCrownPlayerController.h"
#include "NetherCrown/Core/NetherCrownLevelTravelManagerSubsystem.h"
#include "NetherCrown/Core/NetherCrownLevelTravelPersistenceSubsystem.h"
#include "NetherCrown/Data/NetherCrownLevelTravelDataAsset.h"
#include "NetherCrown/PlayerState/NetherCrownPlayerState.h"

void ANetherCrownGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	EnsurePersistentPlayerId(NewPlayer);
}

void ANetherCrownGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	EnsurePersistentPlayerId(C);
}

void ANetherCrownGameMode::TravelLevelByTag(const FGameplayTag& LevelTag)
{
	UGameInstance* GameInstance{ GetGameInstance() };
	if (!ensureAlways(IsValid(GameInstance)))
	{
		return;
	}

	const UNetherCrownLevelTravelManagerSubsystem* LevelTravelManagerSubsystem{ GameInstance->GetSubsystem<UNetherCrownLevelTravelManagerSubsystem>() };
	if (!ensureAlways(IsValid(LevelTravelManagerSubsystem)))
	{
		return;
	}

	const UNetherCrownLevelTravelDataAsset* LevelTravelDataAsset{ LevelTravelManagerSubsystem->GetLevelTravelData(LevelTag) };
	if (!LevelTravelDataAsset)
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("Cannot Found LevelTravel Data!"));
		return;
	}

	const FSoftObjectPath& LevelPathSoft{ LevelTravelDataAsset->GetLevelTravelData().LevelPathSoft };

	UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	const FString LevelObjectPath{ LevelPathSoft.ToString() };
	const FString LevelPackageName{ FPackageName::ObjectPathToPackageName(LevelObjectPath) };

	if (!FPackageName::IsValidLongPackageName(LevelPackageName))
	{
		UE_LOG(
				LogNetherCrown,
				Warning,
				TEXT("Invalid level package name: %s from object path: %s"),
				*LevelPackageName,
				*LevelObjectPath
		);
		return;
	}

	UNetherCrownLevelTravelPersistenceSubsystem* LevelTravelPersistenceSubsystem{ GameInstance->GetSubsystem<UNetherCrownLevelTravelPersistenceSubsystem>() };
	if (IsValid(LevelTravelPersistenceSubsystem))
	{
		LevelTravelPersistenceSubsystem->BeginLevelTravelPersistence();
	}

	for (FConstPlayerControllerIterator PlayerControllerIterator{ World->GetPlayerControllerIterator() }; PlayerControllerIterator; ++PlayerControllerIterator)
	{
		ANetherCrownPlayerController* NetherCrownPlayerController{ Cast<ANetherCrownPlayerController>(PlayerControllerIterator->Get()) };
		if (!IsValid(NetherCrownPlayerController))
		{
			continue;
		}

		NetherCrownPlayerController->Client_BeginLevelTravelPersistence();
	}

	bUseSeamlessTravel = true;
	World->ServerTravel(LevelPackageName);
}

void ANetherCrownGameMode::EnsurePersistentPlayerId(AController* Controller) const
{
	ANetherCrownPlayerController* NetherCrownPlayerController{ Cast<ANetherCrownPlayerController>(Controller) };
	if (!IsValid(NetherCrownPlayerController))
	{
		return;
	}

	FGuid PersistentPlayerId{ NetherCrownPlayerController->GetPersistentPlayerId() };
	if (!PersistentPlayerId.IsValid())
	{
		PersistentPlayerId = FGuid::NewGuid();
		NetherCrownPlayerController->SetPersistentPlayerId(PersistentPlayerId);
	}

	ANetherCrownPlayerState* NetherCrownPlayerState{ NetherCrownPlayerController->GetPlayerState<ANetherCrownPlayerState>() };
	if (!IsValid(NetherCrownPlayerState))
	{
		return;
	}

	if (!NetherCrownPlayerState->GetPersistentPlayerId().IsValid())
	{
		NetherCrownPlayerState->SetPersistentPlayerId(PersistentPlayerId);
		return;
	}

	NetherCrownPlayerController->SetPersistentPlayerId(NetherCrownPlayerState->GetPersistentPlayerId());
}
