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

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag FrozenBasicAttackImpactEffectTag{};
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
struct FNetherCrownBasicAttackCosmeticData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float HitStopAnimRate{ 0.12f };

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float HitStopDuration{ 0.09f };

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float LastComboPlayRate{ 0.15f };

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float LastComboPlayRateDuration{ 0.1f };

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float HitPushInDistance{ 20.f };

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float HitPushInDuration{ 0.08f };

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float LastAttackPPDuration{ 1.f };
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

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float LastComboAttackKnockBackVelocity{ 2000.f };

	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FNetherCrownBasicAttackTagData BasicAttackTagData{};

	UPROPERTY(EditDefaultsOnly, Category = "Cosmetic")
	FNetherCrownBasicAttackCosmeticData BasicAttackCosmeticData{};
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
