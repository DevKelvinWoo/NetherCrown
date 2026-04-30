// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NetherCrownEnemyAITuningData.generated.h"

class UBehaviorTree;

USTRUCT()
struct FNetherCrownEnemyPerceptionData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Perception")
	float SightRadius{ 1500.0f };

	UPROPERTY(EditDefaultsOnly, Category = "Perception")
	float LoseSightRadius{ 2000.0f };

	UPROPERTY(EditDefaultsOnly, Category = "Perception")
	float PeripheralVisionAngleDegrees{ 360.0f };

	UPROPERTY(EditDefaultsOnly, Category = "Perception")
	float MaxSightAge{ 2.0f };
};

USTRUCT()
struct FNetherCrownEnemyAITuningData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Perception")
	FNetherCrownEnemyPerceptionData PerceptionData{};

	UPROPERTY(EditDefaultsOnly, Category = "BlackboardKeyName")
	FName TargetActorBlackboardKeyName{ TEXT("TargetActor") };

	UPROPERTY(EditDefaultsOnly, Category = "BlackboardKeyName")
	FName NeedFoundReactionBlackboardKeyName{ TEXT("bNeedFoundReaction") };

	UPROPERTY(EditDefaultsOnly, Category = "BlackboardKeyName")
	FName ChaseTypeBlackboardKeyName{ TEXT("ChaseType") };

	UPROPERTY(EditDefaultsOnly, Category = "BT")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset{};

	UPROPERTY(EditDefaultsOnly, Category = "Patrol")
	float PatrolRadius{ 1000.0f };
};

USTRUCT()
struct FNetherCrownBossRangedCombatData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "RangedBoss")
	float MinAttackDistance{ 650.0f };

	UPROPERTY(EditDefaultsOnly, Category = "RangedBoss")
	float PreferredAttackDistance{ 1000.0f };

	UPROPERTY(EditDefaultsOnly, Category = "RangedBoss")
	float MaxAttackDistance{ 1350.0f };

	UPROPERTY(EditDefaultsOnly, Category = "RangedBoss")
	float OrbitAngleDegrees{ 70.0f };

	UPROPERTY(EditDefaultsOnly, Category = "RangedBoss")
	float LocationSearchRadius{ 350.0f };

	UPROPERTY(EditDefaultsOnly, Category = "RangedBoss")
	float MinLocationOffset{ 150.0f };
};

UCLASS()
class NETHERCROWN_API UNetherCrownEnemyAITuningDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownEnemyAITuningData& GetEnemyAITuningData() const { return EnemyAITuningData; }
	const FNetherCrownBossRangedCombatData& GetBossRangedCombatData() const { return BossRangedCombatData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "EnemyAITuningData")
	FNetherCrownEnemyAITuningData EnemyAITuningData{};

	UPROPERTY(EditDefaultsOnly, Category = "BossRangedCombatData")
	FNetherCrownBossRangedCombatData BossRangedCombatData{};
};
