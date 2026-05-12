// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "NetherCrownEnemySkillData.generated.h"

class ANetherCrownEnemyCrownPrisonWall;
class UNiagaraSystem;
class UAnimMontage;
class UNetherCrownEnemyBasicAttackDataAsset;

USTRUCT()
struct FNetherCrownEnemySkillTagData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FGameplayTag HitImpactEffectTag{};

	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FGameplayTag HitImpactSoundTag{};
};

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

	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FNetherCrownEnemySkillTagData EnemySkillTagData{};
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
struct FNetherCrownMagicSpikeTagData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FGameplayTag SpikeEffectTag{};

	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FGameplayTag SpikeSoundTag{};

	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FGameplayTag RangeEffectTag{};

	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FGameplayTag RangeSoundTag{};
};

USTRUCT()
struct FNetherCrownMagicSpikeData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "MagicSpikeData")
	FNetherCrownMagicSpikeTagData MagicSpikeTagData{};

	UPROPERTY(EditDefaultsOnly, Category = "MagicSpikeData")
	float MagicSpikeSpawnRadius{ 1000.f };

	UPROPERTY(EditDefaultsOnly, Category = "MagicSpikeData")
	float MagicSpikeRadius{ 200.f };

	UPROPERTY(EditDefaultsOnly, Category = "MagicSpikeData")
	int32 MagicSpikeMaxCount{ 5 };

	UPROPERTY(EditDefaultsOnly, Category = "MagicSpikeData")
	float MagicSpikeRangeInterval{ 0.5f };

	UPROPERTY(EditDefaultsOnly, Category = "MagicSpikeData")
	float MagicSpikeImpactTimeOffset{ 2.f };

	UPROPERTY(EditDefaultsOnly, Category = "MagicSpikeData")
	float MagicSpikeStunDuration{ 2.f };
};

USTRUCT()
struct FNetherCrownEnemyShockwaveRepulseData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "ShockwaveRepulseData")
	float ShockwaveRepulseDuration{ 2.f };

	UPROPERTY(EditDefaultsOnly, Category = "ShockwaveRepulseData")
	float ShockwaveRepulseKnockbackTimeOffset{ 0.45f };

	UPROPERTY(EditDefaultsOnly, Category = "ShockwaveRepulseData")
	float ShockwaveRepulseKnockbackDistance{ 1200.f };
};

USTRUCT()
struct FNetherCrownEnemyTeleportTagData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FGameplayTag TeleportSoundTag{};

	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FGameplayTag TeleportEffectTag{};
};

USTRUCT()
struct FNetherCrownEnemyTeleportData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "TeleportData")
	FNetherCrownEnemyTeleportTagData TeleportTagData{};

	UPROPERTY(EditDefaultsOnly, Category = "TeleportData")
	float TeleportDistance{ 500.f };

	UPROPERTY(EditDefaultsOnly, Category = "TeleportData")
	float TeleportRandomRadius{ 30.f };
};

USTRUCT()
struct FNetherCrownEnemyCrownPrisonTagData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FGameplayTag CrownPrisonStatueRiseSoundTag{};

	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FGameplayTag CrownPrisonRangedSoundTag{};

	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FGameplayTag CrownPrisonExplosionSoundTag{};
};

USTRUCT()
struct FNetherCrownEnemyCrownPrisonData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "CrownPrisonData")
	TSubclassOf<ANetherCrownEnemyCrownPrisonWall> CrownPrisonWallClass{};

	UPROPERTY(EditDefaultsOnly, Category = "CrownPrisonData")
	FNetherCrownEnemyCrownPrisonTagData CrownPrisonTagData{};

	UPROPERTY(EditDefaultsOnly, Category = "CrownPrisonData")
	float WallHiddenZOffset{ -300.f };

	UPROPERTY(EditDefaultsOnly, Category = "CrownPrisonData")
	float WallRiseDuration{ 1.5f };

	UPROPERTY(EditDefaultsOnly, Category = "CrownPrisonData")
	float ExplosionRadius{ 700.f };

	UPROPERTY(EditDefaultsOnly, Category = "CrownPrisonData")
	float ExplosionTimeOffset{ 3.f };

	UPROPERTY(EditDefaultsOnly, Category = "CrownPrisonData")
	float CrownPrisonStunDuration{ 2.f };
};

USTRUCT()
struct FNetherCrownEnemyVoidPiercerTagData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FGameplayTag HitImpactEffectTag{};
};

USTRUCT()
struct FNetherCrownEnemyVoidPiercerData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "EnemyVoidPiercerData")
	float JumpToSkyDuration{ 0.5f };

	UPROPERTY(EditDefaultsOnly, Category = "EnemyVoidPiercerData")
	float JumpToSkyOffset{ 500.f };

	UPROPERTY(EditDefaultsOnly, Category = "EnemyVoidPiercerData")
	float FlyingDuration{ 4.f };

	UPROPERTY(EditDefaultsOnly, Category = "EnemyVoidPiercerData")
	float DamageInterval{ 0.5f };

	UPROPERTY(EditDefaultsOnly, Category = "EnemyVoidPiercerData")
	float LaserRadius{ 30.f };

	UPROPERTY(EditDefaultsOnly, Category = "EnemyVoidPiercerData")
	float LaserRange{ 1200.f };

	UPROPERTY(EditDefaultsOnly, Category = "EnemyVoidPiercerData")
	FNetherCrownEnemyVoidPiercerTagData VoidPiercerTagData{};
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
class NETHERCROWN_API UNetherCrownEnemyMagicSpikeDataAsset : public UNetherCrownEnemySkillDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownMagicSpikeData& GetEnemyMagicSpikeData() const { return EnemyMagicSpikeData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "EnemyTeleportData")
	FNetherCrownMagicSpikeData EnemyMagicSpikeData{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownEnemyShockwaveRepulseDataAsset : public UNetherCrownEnemySkillDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownEnemyShockwaveRepulseData& GetEnemyShockwaveRepulseData() const { return EnemyShockwaveRepulseData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "EnemyTeleportData")
	FNetherCrownEnemyShockwaveRepulseData EnemyShockwaveRepulseData{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownEnemyTeleportDataAsset : public UNetherCrownEnemySkillDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownEnemyTeleportData& GetEnemyTeleportData() const { return EnemyTeleportData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "EnemyTeleportData")
	FNetherCrownEnemyTeleportData EnemyTeleportData{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownEnemyCrownPrisonDataAsset : public UNetherCrownEnemySkillDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownEnemyCrownPrisonData& GetEnemyCrownPrisonData() const { return EnemyCrownPrisonData; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "EnemyCrownPrisonData")
	FNetherCrownEnemyCrownPrisonData EnemyCrownPrisonData{};
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
