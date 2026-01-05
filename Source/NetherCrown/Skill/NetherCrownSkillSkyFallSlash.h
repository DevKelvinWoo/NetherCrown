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
	void ApplySkillCameraCurveFloat();
	void StartSkillCameraCurveTimer();

	void StartSkillArmMaterialParameterCurveTimer();
	void ApplySkillArmMaterialParameterCurveFloat();

	void HandleOnHitSkyFallSlashSkill();
	const TArray<ANetherCrownEnemy*> GetSkillDetectedTargets() const;

	FTimerHandle SkillCameraCurveTimerHandle{};
	float SkillCameraCurveElapsedTime{ 0.f };

	FTimerHandle SkillArmMaterialCurveTimerHandle{};
	float SkillArmMaterialCurveElapsedTime{ 0.f };

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UCurveFloat> SkillCameraCurveFloatSoft{};

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UCurveFloat> SkillArmMaterialCurveFloatSoft{};

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCameraShakeBase> SkillCameraShakeBaseClass{};

	const float SkillDetectingSphereRadius{ 225.f };
	const double SkillDetectingThresholdDegrees{ 45.0 };
	const FVector SkillKnockBackVector{ FVector(-2400.f, 0.f, 0.f) };
	const float KnockBackDuration{ 1.f };

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ArmMaterialInstanceDynamic{};

	const FName ArmMaterialScalarParameterName{ "SkyFallSlashAlpha" };
};
