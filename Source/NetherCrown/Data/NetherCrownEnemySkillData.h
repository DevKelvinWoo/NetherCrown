// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NetherCrown/Enemy/EnemySkill/NetherCrownEnemySkillObject.h"
#include "NetherCrownEnemySkillData.generated.h"

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
struct FNetherCrownEnemyDashAttackData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "EnemyDashAttackData")
	float DashDuration{ 0.3f };

	UPROPERTY(EditDefaultsOnly, Category = "EnemyDashAttackData")
	float DashAttackDuration{ 1.5f };

	UPROPERTY(EditDefaultsOnly, Category = "EnemyDashAttackData")
	float DashOffset{ 100.f };
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
