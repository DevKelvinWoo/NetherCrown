// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NetherCrown/Components/NetherCrownQuestComponent.h"
#include "NetherCrown/Data/NetherCrownWeaponData.h"
#include "NetherCrownLevelTravelPersistenceSubsystem.generated.h"

class UNetherCrownLoadingScreenWidgetView;

UCLASS()
class NETHERCROWN_API UNetherCrownLevelTravelPersistenceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void BeginLevelTravelPersistence();
	void RestoreLevelTravelPersistence();

private:
	void CaptureWeaponPersistentData();
	bool RestoreWeaponPersistentData();
	void CaptureQuestPersistentData();
	bool RestoreQuestPersistentData();
	void ShowLoadingScreen();
	void HideLoadingScreen();

	UPROPERTY(Transient)
	TArray<TObjectPtr<UNetherCrownLoadingScreenWidgetView>> LoadingScreenWidgets{};

	UPROPERTY(Transient)
	TMap<FGuid, FNetherCrownWeaponPersistentData> WeaponPersistentDataMap{};

	UPROPERTY(Transient)
	TArray<FGuid> WeaponPersistentPlayerIdQueue{};

	UPROPERTY(Transient)
	TMap<FGuid, FNetherCrownQuestPersistentData> QuestPersistentDataMap{};

	UPROPERTY(Transient)
	TArray<FGuid> QuestPersistentPlayerIdQueue{};

	bool bHasPendingPersistence{ false };
};
