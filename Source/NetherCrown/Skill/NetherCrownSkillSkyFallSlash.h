// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownSkillObject.h"
#include "Components/TimelineComponent.h"
#include "NetherCrownSkillSkyFallSlash.generated.h"

class UCurveFloat;

class ANetherCrownEnemy;

UCLASS(Blueprintable)
class NETHERCROWN_API UNetherCrownSkillSkyFallSlash : public UNetherCrownSkillObject
{
	GENERATED_BODY()

public:
	UNetherCrownSkillSkyFallSlash();

protected:
	virtual void InitSkillObject() override;
	virtual void ExecuteSkillGameplay() override;
	virtual void PlaySkillCosmetics() override;

	virtual void TickFloatTimeline(float DeltaTime) override;

private:
	void StartSetSpringArmZOffsetTimeline();
	void StartSetArmMaterialParamTimeline();
	void BindTimelineFunctions();

	void SetupSkyFallSlashHitTimers();

	UFUNCTION(Client, Unreliable)
	void Client_StartCameraShake();

	UFUNCTION()
	void SetSpringArmZOffsetByFloatTimeline(float FloatCurveValue);

	UFUNCTION()
	void SetArmMaterialParamByFloatTimeline(float FloatCurveValue);

	void HandleOnHitSkyFallSlashSkill();

	void DetectAndHitSkyFallSlashSkill();
	const TArray<ANetherCrownEnemy*> GetSkillDetectedTargets() const;

	void CreateArmMaterialInstanceDynamic();

	UPROPERTY(EditDefaultsOnly, Category = "CurveData")
	TSoftObjectPtr<UCurveFloat> SkillCameraCurveFloatSoft{};

	UPROPERTY(EditDefaultsOnly, Category = "CurveData")
	TSoftObjectPtr<UCurveFloat> SkillArmMaterialCurveFloatSoft{};

	UPROPERTY(EditDefaultsOnly, Category = "CameraShakeClass")
	TSubclassOf<UCameraShakeBase> SkillCameraShakeBaseClass{};

	UPROPERTY(EditDefaultsOnly, Category = "Skill Settings|Range")
	float SkillDetectingSphereRadius{ 225.f };

	UPROPERTY(EditDefaultsOnly, Category = "Skill Settings|Range")
	double SkillDetectingThresholdDegrees{ 45.0 };

	UPROPERTY(EditDefaultsOnly, Category = "Skill Settings|Range")
	float SkillKnockBackDistance{ 2400.f };

	UPROPERTY(EditDefaultsOnly, Category = "Skill Settings|Range")
	float KnockBackDuration{ 1.f };

	UPROPERTY(EditDefaultsOnly, Category = "MaterialParam")
	FName ArmMaterialScalarParameterName{ TEXT("SkyFallSlashAlpha") };

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> CachedSkillCameraCurveFloat{};

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> CachedSkillArmMaterialCurveFloat{};

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> ArmMaterialInstanceDynamic{};

	FTimeline SpringArmZOffsetFloatTimeline{};
	FTimeline ArmMaterialFloatTimeline{};
};
