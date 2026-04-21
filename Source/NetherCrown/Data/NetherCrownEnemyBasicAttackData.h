// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NetherCrownEnemyBasicAttackData.generated.h"

class UAnimMontage;

UENUM()
enum class ENetherCrownEnemyBasicAttackType : uint8
{
	BasicAttack,
	DashAttack,
};

USTRUCT()
struct FNetherCrownEnemyBasicAttackComboData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "ComboData")
	FName ComboMontageSectionName{};

	UPROPERTY(EditDefaultsOnly, Category = "TraceData")
	float EnableHitTraceTime{ 0.f };

	UPROPERTY(EditDefaultsOnly, Category = "TraceData")
	float DisableHitTraceTime{ 0.f };

	UPROPERTY(EditDefaultsOnly, Category = "TraceData")
	float EndAttackTime{ 0.f };
};

USTRUCT()
struct FNetherCrownEnemyBasicAttackData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "AnimMontage")
	TSoftObjectPtr<UAnimMontage> BasicAttackMontageSoft{};

	UPROPERTY(EditDefaultsOnly, Category = "ComboData")
	TMap<int32, FNetherCrownEnemyBasicAttackComboData> EnemyComboDataMap{};

	UPROPERTY(EditDefaultsOnly, Category = "TraceData")
	FName WeaponTraceSocketName{};

	UPROPERTY(EditDefaultsOnly, Category = "TraceData")
	float TraceRadius{ 10.f };

	UPROPERTY(EditDefaultsOnly, Category = "BasicAttackType")
	ENetherCrownEnemyBasicAttackType BasicAttackType{ ENetherCrownEnemyBasicAttackType::BasicAttack };
};

UCLASS()
class NETHERCROWN_API UNetherCrownEnemyBasicAttackDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownEnemyBasicAttackData& GetEnemyBasicAttackData() const { return EnemyBasicAttackData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "EnemyBasicAttackData")
	FNetherCrownEnemyBasicAttackData EnemyBasicAttackData{};
};
