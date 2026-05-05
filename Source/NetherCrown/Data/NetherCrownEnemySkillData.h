// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "NetherCrownEnemySkillData.generated.h"

class UAnimMontage;
class UNetherCrownEnemyBasicAttackDataAsset;

USTRUCT()
struct FNetherCrownEnemySkillData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "EnemySkillAnim")
	TSoftObjectPtr<UAnimMontage> EnemySkillAnimMontageSoft{};

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	int32 EnemySkillDamage{ 10 };

	UPROPERTY(EditDefaultsOnly, Category = "Duration")
	float EnemySkillDuration{ 1.5f };

	UPROPERTY(EditDefaultsOnly, Category = "EnemySkillCoolDown")
	float EnemySkillCoolDown{ 3.f };
};

UCLASS()
class NETHERCROWN_API UNetherCrownEnemySkillDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownEnemySkillData& GetEnemySkillData() const { return EnemySkillData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "EnemySkillData")
	FNetherCrownEnemySkillData EnemySkillData{};
};

USTRUCT()
struct FNetherCrownEnemySkillDataTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	const FGameplayTag& GetEnemySkillTag() const { return EnemySkillTag; }
	TSoftObjectPtr<UNetherCrownEnemySkillDataAsset> GetEnemySkillDataAsset() const { return EnemySkillDataAssetSoft; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "EnemySkillTag")
	FGameplayTag EnemySkillTag{};

	UPROPERTY(EditDefaultsOnly, Category = "EnemySkillData")
	TSoftObjectPtr<UNetherCrownEnemySkillDataAsset> EnemySkillDataAssetSoft{};
};

USTRUCT()
struct FNetherCrownEnemyVoidPiercerData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "EnemyVoidPiecerData")
	float JumpToSkyDuration{ 0.5f };

	UPROPERTY(EditDefaultsOnly, Category = "EnemyVoidPiecerData")
	float JumpToSkyOffset{ 500.f };

	UPROPERTY(EditDefaultsOnly, Category = "EnemyVoidPiecerData")
	float FlyingDuration{ 4.f };

	UPROPERTY(EditDefaultsOnly, Category = "EnemyVoidPiecerData")
	float DamageInterval{ 0.5f };

	UPROPERTY(EditDefaultsOnly, Category = "EnemyVoidPiecerData")
	float LaserRadius{ 30.f };

	UPROPERTY(EditDefaultsOnly, Category = "EnemyVoidPiecerData")
	float LaserRange{ 1200.f };
};

USTRUCT()
struct FNetherCrownEnemyDashAttackData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "EnemyDashAttackData")
	float DashDuration{ 0.3f };

	UPROPERTY(EditDefaultsOnly, Category = "EnemyDashAttackData")
	float DashOffset{ 100.f };

	UPROPERTY(EditDefaultsOnly, Category = "EnemyDashAttackData")
	float DashKnockBackDistance{ 1000.f };

	UPROPERTY(EditDefaultsOnly, Category = "EnemyDashAttackData")
	float DashKnockBackDuration{ 2.f };

	UPROPERTY(EditDefaultsOnly, Category = "EnemyDashAttackData")
	TSoftObjectPtr<UNetherCrownEnemyBasicAttackDataAsset> DashFollowUpBasicAttackDataAssetSoft{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownEnemyVoidPiercerDataAsset : public UNetherCrownEnemySkillDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownEnemyVoidPiercerData& GetEnemyVoidPiercerData() const { return EnemyVoidPiercerData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "EnemyVoidPiercerData")
	FNetherCrownEnemyVoidPiercerData EnemyVoidPiercerData{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownEnemyDashAttackDataAsset : public UNetherCrownEnemySkillDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownEnemyDashAttackData& GetEnemyDashAttackData() const { return EnemyDashAttackData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "EnemyDashAttackData")
	FNetherCrownEnemyDashAttackData EnemyDashAttackData{};
};
