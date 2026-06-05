// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "NetherCrownInteractData.generated.h"

UCLASS()
class NETHERCROWN_API UNetherCrownInteractActorDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const FText& GetDialogueText() const { return DialogueText; }
	const TSoftObjectPtr<UTexture2D>& GetInteractWidgetTexture() const { return InteractWidgetTextureSoft; }
	const FGameplayTag& GetInteractActorTag() const { return InteractActorTag; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
	FText DialogueText{};

	UPROPERTY(EditDefaultsOnly, Category = "InteractWidget")
	TSoftObjectPtr<UTexture2D> InteractWidgetTextureSoft{};

	UPROPERTY(EditDefaultsOnly, Category = "Tag")
	FGameplayTag InteractActorTag{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownPortalDataAsset : public UNetherCrownInteractActorDataAsset
{
	GENERATED_BODY()

public:
	const FGameplayTag& GetLevelTag() const { return LevelTag; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Tag")
	FGameplayTag LevelTag{};
};

USTRUCT()
struct FNetherCrownBossDungeonDoorTagData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FGameplayTag OpenDoorSoundTag{};
};

USTRUCT()
struct FNetherCrownBossDungeonDoorData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Curve")
	TSoftObjectPtr<UCurveFloat> LeftDoorOpenCurve{};

	UPROPERTY(EditDefaultsOnly, Category = "Curve")
	TSoftObjectPtr<UCurveFloat> RightDoorOpenCurve{};

	UPROPERTY(EditDefaultsOnly, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> OpenDoorCameraShakeClass{};

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	float OpenDoorCameraShakeInnerRadius{ 1500.f };

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	float OpenDoorCameraShakeOuterRadius{ 2000.f };

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	FNetherCrownBossDungeonDoorTagData BossDungeonDoorTagData{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownBossDungeonDoorDataAsset : public UNetherCrownInteractActorDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownBossDungeonDoorData& GetBossDungeonDoorData() const { return BossDungeonDoorData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "BossDungeonDoorData")
	FNetherCrownBossDungeonDoorData BossDungeonDoorData{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownInteractData : public UDataAsset
{
	GENERATED_BODY()
};
