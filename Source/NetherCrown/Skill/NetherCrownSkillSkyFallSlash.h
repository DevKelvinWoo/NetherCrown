// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownSkillObject.h"
#include "NetherCrownSkillSkyFallSlash.generated.h"

class UCurveFloat;

class ANetherCrownEnemy;

UCLASS(Blueprintable)
class NETHERCROWN_API UNetherCrownSkillSkyFallSlash : public UNetherCrownSkillObject
{
	GENERATED_BODY()

protected:
	virtual void InitSkillObject() override;
	virtual void ExecuteSkillGameplay() override;
	virtual void PlaySkillCosmetics() override;

private:
	void StartSkillCameraCurveTimer();
	void ApplySkillCameraCurveFloat();
	void SetSpringArmZOffset();

	void StartSkillArmMaterialParameterCurveTimer();
	void ApplySkillArmMaterialParameterCurveFloat();
	void SetSkillArmMaterialScalarParam();

	void HandleOnHitSkyFallSlashSkill();
	const TArray<ANetherCrownEnemy*> GetSkillDetectedTargets() const;

	void CreateArmMaterialInstanceDynamic();

	FTimerHandle SkillCameraCurveTimerHandle{};
	float SkillCameraCurveElapsedTime{ 0.f };

	FTimerHandle SkillArmMaterialCurveTimerHandle{};
	float SkillArmMaterialCurveElapsedTime{ 0.f };

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UCurveFloat> SkillCameraCurveFloatSoft{};

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UCurveFloat> SkillArmMaterialCurveFloatSoft{};

	UPROPERTY()
	TObjectPtr<UCurveFloat> CachedSkillCameraCurveFloat{};

	UPROPERTY()
	TObjectPtr<UCurveFloat> CachedSkillArmMaterialCurveFloat{};

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCameraShakeBase> SkillCameraShakeBaseClass{};

	UPROPERTY(EditDefaultsOnly, Category = "Skill Settings|Range")
	float SkillDetectingSphereRadius{ 225.f };

	UPROPERTY(EditDefaultsOnly, Category = "Skill Settings|Range")
	double SkillDetectingThresholdDegrees{ 45.0 };

	UPROPERTY(EditDefaultsOnly, Category = "Skill Settings|Range")
	float SkillKnockBackDistance{ 2400.f };

	UPROPERTY(EditDefaultsOnly, Category = "Skill Settings|Range")
	float KnockBackDuration{ 1.f };

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ArmMaterialInstanceDynamic{};

	const FName ArmMaterialScalarParameterName{ TEXT("SkyFallSlashAlpha") };
};
