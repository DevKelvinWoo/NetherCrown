// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "NetherCrownParryData.generated.h"

USTRUCT()
struct FNetherCrownParryTagData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "ParryTagData")
	FGameplayTag ParrySoundTag{};

	UPROPERTY(EditDefaultsOnly, Category = "ParryTagData")
	FGameplayTag ParryEffectTag{};
};

USTRUCT()
struct FNetherCrownParryData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "ParryData")
	FNetherCrownParryTagData ParryTagData{};

	UPROPERTY(EditDefaultsOnly, Category = "ParryData")
	float ParryStunDuration{ 2.5f };

	UPROPERTY(EditDefaultsOnly, Category = "ParryData")
	float ParryThresholdDegrees{ 45.f };

	UPROPERTY(EditDefaultsOnly, Category = "ParryData")
	TSubclassOf<UCameraShakeBase> ParryCameraShakeClass{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownParryDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownParryData& GetParryData() const { return ParryData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "ParryData")
	FNetherCrownParryData ParryData{};
};
