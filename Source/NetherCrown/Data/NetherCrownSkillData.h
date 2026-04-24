// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "NetherCrown/Skill/NetherCrownSkillObject.h"
#include "NetherCrownSkillData.generated.h"

class UCameraShakeBase;
class UCurveFloat;
class UCurveVector;
class UMaterialInterface;
class UNiagaraSystem;

class ANetherCrownShield;

USTRUCT()
struct FNetherCrownDodgeSkillData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "SkillData")
	float DodgeDistance{ 500.f };
};

USTRUCT()
struct FNetherCrownShieldMasteryData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "SkillData")
	int32 ShieldDuration{ 10 };

	UPROPERTY(EditAnywhere, Category = "SkillData")
	int32 ShieldValue{ 50 };

	UPROPERTY(EditAnywhere, Category = "SkillData")
	FName ShieldEffectSocketName{ TEXT("root") };

	UPROPERTY(EditAnywhere, Category = "ShieldActor")
	TSubclassOf<ANetherCrownShield> ShieldActorClass{};
};

USTRUCT()
struct FNetherCrownSkyFallSlashData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "CurveData")
	TSoftObjectPtr<UCurveFloat> SkillCameraCurveFloatSoft{};

	UPROPERTY(EditAnywhere, Category = "CurveData")
	TSoftObjectPtr<UCurveFloat> SkillArmMaterialCurveFloatSoft{};

	UPROPERTY(EditAnywhere, Category = "CameraShakeClass")
	TSubclassOf<UCameraShakeBase> SkillCameraShakeBaseClass{};

	UPROPERTY(EditAnywhere, Category = "Skill Settings|Range")
	float SkillDetectingSphereRadius{ 225.f };

	UPROPERTY(EditAnywhere, Category = "Skill Settings|Range")
	double SkillDetectingThresholdDegrees{ 45.0 };

	UPROPERTY(EditAnywhere, Category = "Skill Settings|Range")
	float SkillKnockBackDistance{ 2400.f };

	UPROPERTY(EditAnywhere, Category = "Skill Settings|Range")
	float KnockBackDuration{ 1.f };

	UPROPERTY(EditAnywhere, Category = "MaterialParam")
	FName ArmMaterialScalarParameterName{ TEXT("SkyFallSlashAlpha") };
};

USTRUCT()
struct FNetherCrownFrozenTempestData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "CurveData")
	TSoftObjectPtr<UCurveVector> SkillCameraZoomCurveVectorSoft{};

	UPROPERTY(EditAnywhere, Category = "CurveData")
	TSoftObjectPtr<UCurveFloat> CharacterOverlayMaterialStartCurveFloatSoft{};

	UPROPERTY(EditAnywhere, Category = "CurveData")
	TSoftObjectPtr<UCurveFloat> CharacterOverlayMaterialEndCurveFloatSoft{};

	UPROPERTY(EditAnywhere, Category = "CameraShakeClass")
	TSubclassOf<UCameraShakeBase> SkillCameraShakeBaseClass{};

	UPROPERTY(EditAnywhere, Category = "CameraShakeClass")
	TSubclassOf<UCameraShakeBase> SkillChargeCameraShakeBaseClass{};

	UPROPERTY(EditAnywhere, Category = "Skill Settings|Range")
	float SkillDetectingSphereRadius{ 500.f };

	UPROPERTY(EditAnywhere, Category = "Skill Settings|Duration")
	float FrozenDuration{ 5.f };

	UPROPERTY(EditAnywhere, Category = "Skill Settings|Duration")
	float ChargingPostProcessDuration{ 2.5f };

	UPROPERTY(EditAnywhere, Category = "MaterialData")
	TSoftObjectPtr<UMaterialInterface> FrozenTempestTargetOverlayMaterialSoft{};

	UPROPERTY(EditAnywhere, Category = "MaterialParam")
	FName FrozenTempestTargetMaterialParam{ TEXT("_VfxMix") };
};

USTRUCT()
struct FNetherCrownDashAttackData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "SkillData")
	float SkillDetectSphereOffset{ 750.f };

	UPROPERTY(EditAnywhere, Category = "SkillData")
	float EndLocationOffset{ 50.f };

	UPROPERTY(EditAnywhere, Category = "SkillData")
	float DashTimerRate{ 0.4f };

	UPROPERTY(EditAnywhere, Category = "SkillData")
	float DashDuration{ 0.2f };

	UPROPERTY(EditAnywhere, Category = "SkillData")
	int32 MaxTargetNum{ 5 };

	UPROPERTY(EditAnywhere, Category = "SkillData")
	float StunDuration{ 3.f };

	UPROPERTY(EditAnywhere, Category = "SkillData")
	float LastDashAttackTimeOffset{ 1.f };

	UPROPERTY(EditAnywhere, Category = "SkillData")
	float ChargingPostProcessDuration{ 2.5f };

	UPROPERTY(EditAnywhere, Category = "CameraData")
	float LastDashAttackCameraDistance{ 500.f };

	UPROPERTY(EditAnywhere, Category = "CameraData")
	float LastDashAttackCameraBlendTime{ 0.5f };

	UPROPERTY(EditAnywhere, Category = "CameraData")
	float LastDashAttackCameraBlendExp{ 2.f };

	UPROPERTY(EditAnywhere, Category = "CameraData")
	float LastDashAttackCameraRestoreDuration{ 2.f };

	UPROPERTY(EditAnywhere, Category = "NiagaraSystem")
	TSoftObjectPtr<UNiagaraSystem> GhostTrailNiagaraSystemSoft{};

	UPROPERTY(EditAnywhere, Category = "CameraShakeClass")
	TSubclassOf<UCameraShakeBase> DashAttackHitCameraShakeClass{};

	UPROPERTY(EditAnywhere, Category = "AnimMontage")
	TSoftObjectPtr<UAnimMontage> LastDashAttackAnimMontageSoft{};

	UPROPERTY(EditAnywhere, Category = "CurveData")
	TSoftObjectPtr<UCurveVector> DashAttackCameraPosBeginCurveSoft{};

	UPROPERTY(EditAnywhere, Category = "CurveData")
	TSoftObjectPtr<UCurveVector> DashAttackCameraPosEndCurveSoft{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownSkillDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownSkillData& GetSkillData() const { return SkillData; }

private:
	UPROPERTY(EditAnywhere, Category = "SkillData")
	FNetherCrownSkillData SkillData{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownDodgeSkillDataAsset : public UNetherCrownSkillDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownDodgeSkillData& GetDodgeSkillData() const { return DodgeSkillData; }

private:
	UPROPERTY(EditAnywhere, Category = "DodgeSkillData")
	FNetherCrownDodgeSkillData DodgeSkillData{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownShieldMasteryDataAsset : public UNetherCrownSkillDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownShieldMasteryData& GetShieldMasteryData() const { return ShieldMasteryData; }

private:
	UPROPERTY(EditAnywhere, Category = "ShieldMasteryData")
	FNetherCrownShieldMasteryData ShieldMasteryData{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownSkyFallSlashDataAsset : public UNetherCrownSkillDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownSkyFallSlashData& GetSkyFallSlashData() const { return SkyFallSlashData; }

private:
	UPROPERTY(EditAnywhere, Category = "SkyFallSlashData")
	FNetherCrownSkyFallSlashData SkyFallSlashData{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownFrozenTempestDataAsset : public UNetherCrownSkillDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownFrozenTempestData& GetFrozenTempestData() const { return FrozenTempestData; }

private:
	UPROPERTY(EditAnywhere, Category = "FrozenTempestData")
	FNetherCrownFrozenTempestData FrozenTempestData{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownDashAttackDataAsset : public UNetherCrownSkillDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownDashAttackData& GetDashAttackData() const { return DashAttackData; }

private:
	UPROPERTY(EditAnywhere, Category = "DashAttackData")
	FNetherCrownDashAttackData DashAttackData{};
};

USTRUCT(BlueprintType)
struct FNetherCrownSkillDataTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	const FGameplayTag& GetSkillTag() const { return SkillTag; }
	TSoftObjectPtr<UNetherCrownSkillDataAsset> GetSkillDataAsset() const { return SkillDataAssetSoft; }

private:
	UPROPERTY(EditAnywhere, Category = "SkillTag")
	FGameplayTag SkillTag{};

	UPROPERTY(EditAnywhere, Category = "SkillData")
	TSoftObjectPtr<UNetherCrownSkillDataAsset> SkillDataAssetSoft{};
};
