// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "NetherCrownCharacterDamageReceiveDataAsset.generated.h"

class UCameraShakeBase;

USTRUCT()
struct FNetherCrownDamageReceiveTagData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "DamageReceiveTagData")
	FGameplayTag HitImpactTag{};
};

USTRUCT()
struct FNetherCrownCharacterDamageReceiveData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "DamageReceiveData")
	TSubclassOf<UCameraShakeBase> HitCameraShakeClass{};

	UPROPERTY(EditDefaultsOnly, Category = "DamageReceiveData")
	FNetherCrownDamageReceiveTagData DamageReceiveTagData{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownCharacterDamageReceiveDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownCharacterDamageReceiveData& GetDamageReceiveData() const { return DamageReceiveData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "DamageReceiveData")
	FNetherCrownCharacterDamageReceiveData DamageReceiveData{};
};
