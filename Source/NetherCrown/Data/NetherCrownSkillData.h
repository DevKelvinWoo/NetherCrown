// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "NetherCrown/Skill/NetherCrownSkillObject.h"
#include "NetherCrownSkillData.generated.h"

class UCameraShakeBase;
class UCurveFloat;

class ANetherCrownShield;

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

USTRUCT(BlueprintType)
struct FNetherCrownSkillDataTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	const FGameplayTag& GetSkillTag() const { return SkillTag; }
	TSoftObjectPtr<UNetherCrownSkillDataAsset> GetSkillDataAsset() const { return SkillDataAsset; }

private:
	UPROPERTY(EditAnywhere, Category = "SkillTag")
	FGameplayTag SkillTag{};

	UPROPERTY(EditAnywhere, Category = "SkillData")
	TSoftObjectPtr<UNetherCrownSkillDataAsset> SkillDataAsset{};
};
