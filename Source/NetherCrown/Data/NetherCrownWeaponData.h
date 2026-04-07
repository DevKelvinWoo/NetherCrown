// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "NetherCrownWeaponData.generated.h"

class UNiagaraSystem;
enum class ENetherCrownSkillKeyEnum : uint8;

USTRUCT(BlueprintType)
struct FNetherCrownWeaponTraceData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "TraceData")
	FName WeaponTraceSocketName{ TEXT("TraceEnd") };

	UPROPERTY(EditAnywhere, Category = "TraceData")
	float SwingTraceRadius{ 20.0f };

	UPROPERTY(EditAnywhere, Category = "TraceData")
	float ThrustStartForwardOffset{ 40.0f };

	UPROPERTY(EditAnywhere, Category = "TraceData")
	float ThrustStartUpOffset{ 50.0f };

	UPROPERTY(EditAnywhere, Category = "TraceData")
	float ThrustTraceLength{ 160.0f };

	UPROPERTY(EditAnywhere, Category = "TraceData")
	float ThrustTraceRadius{ 35.0f };
};

USTRUCT(BlueprintType)
struct FNetherCrownWeaponTagData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FGameplayTag WeaponSwingSoundTag{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownWeaponData : public UDataAsset
{
	GENERATED_BODY()

public:
	int32 GetWeaponAttackDamage() const { return WeaponAttackDamage; }
	int32 GetPhysicalPenetration() const { return PhysicalPenetration; }
	int32 GetMagicPenetration() const { return MagicPenetration; }

	const FGameplayTag& GetWeaponSwingSoundTag() const { return WeaponTagData.WeaponSwingSoundTag; }
	const FNetherCrownWeaponTagData& GetWeaponTagData() const { return WeaponTagData; }
	const FNetherCrownWeaponTraceData& GetWeaponTraceData() const { return WeaponTraceData; }
	const TMap<ENetherCrownSkillKeyEnum, TSoftObjectPtr<UNiagaraSystem>>& GetWeaponSkillAuraMap() const { return WeaponSkillAuraMap; }
	const TSoftObjectPtr<UNiagaraSystem>& GetWeaponLastComboAttackAura() { return WeaponLastComboAttackAuraSoft; }

private:
	UPROPERTY(EditAnywhere)
	FString WeaponName{};

	UPROPERTY(EditAnywhere)
	int32 WeaponAttackDamage{};

	UPROPERTY(EditAnywhere)
	int32 WeaponMagicPower{};

	UPROPERTY(EditAnywhere)
	int32 PhysicalPenetration{};

	UPROPERTY(EditAnywhere)
	int32 MagicPenetration{};

	UPROPERTY(EditAnywhere)
	int32 WeaponMovementSpeed{};

	UPROPERTY(EditAnywhere)
	int32 WeaponCoolDownReduction{};

	UPROPERTY(EditAnywhere, Category = "TagData")
	FNetherCrownWeaponTagData WeaponTagData{};

	UPROPERTY(EditAnywhere, Category = "TraceData")
	FNetherCrownWeaponTraceData WeaponTraceData{};

	UPROPERTY(EditAnywhere, Category = "Effect")
	TMap<ENetherCrownSkillKeyEnum, TSoftObjectPtr<UNiagaraSystem>> WeaponSkillAuraMap{};

	UPROPERTY(EditAnywhere, Category = "Effect")
	TSoftObjectPtr<UNiagaraSystem> WeaponLastComboAttackAuraSoft{};
};

USTRUCT(BlueprintType)
struct FNetherCrownWeaponDataTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	const FGameplayTag& GetWeaponTag() const { return WeaponTag; }
	TSoftObjectPtr<UNetherCrownWeaponData> GetWeaponData() const { return WeaponData; }

private:
	UPROPERTY(EditAnywhere, Category = "WeaponTag")
	FGameplayTag WeaponTag{};

	UPROPERTY(EditAnywhere, Category = "WeaponData")
	TSoftObjectPtr<UNetherCrownWeaponData> WeaponData{};
};
