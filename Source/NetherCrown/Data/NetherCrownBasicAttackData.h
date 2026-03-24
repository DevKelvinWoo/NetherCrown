// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "NetherCrownBasicAttackData.generated.h"

class UAnimMontage;
class UCameraShakeBase;

USTRUCT()
struct FNetherCrownBasicAttackTagData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag BasicAttackGruntSoundTag{};

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag BasicAttackImpactEffectTag{};
};

USTRUCT()
struct FNetherCrownBasicAttackComboData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FName ComboMontageSectionName{};

	//@NOTE : Montage Section 시작 기준, 콤보 윈도우가 열리는 시간 (초)
	UPROPERTY(EditDefaultsOnly)
	float ComboWindowOpenTime{ 0.0f };

	//@NOTE : Montage Section 시작 기준, 콤보 윈도우가 닫히는 시간 (초)
	UPROPERTY(EditDefaultsOnly)
	float ComboWindowCloseTime{ 0.0f };

	UPROPERTY(EditDefaultsOnly)
	float AttackEndTime{ 0.0f };

	UPROPERTY(EditDefaultsOnly)
	float HitTraceEnableTime{ 0.0f };
};

USTRUCT()
struct FNetherCrownBasicAttackData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "AnimMontage")
	TSoftObjectPtr<UAnimMontage> BasicAttackAnimMontageSoft{};

	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	TMap<int32, FNetherCrownBasicAttackComboData> ComboDataMap{};

	UPROPERTY(EditDefaultsOnly, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> ApplyDamageCameraShakeClass{};

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float AutoTargetingRadius{ 130.0f };

	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FNetherCrownBasicAttackTagData BasicAttackTagData{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownBasicAttackDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownBasicAttackData& GetBasicAttackData() const { return BasicAttackData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "BasicAttackData")
	FNetherCrownBasicAttackData BasicAttackData{};
};
