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

protected:
	virtual void InitSkillObject() override;
	virtual void ExecuteSkillGameplay() override;
	virtual void PlaySkillCosmetics() override;

	virtual void TickFloatTimeline(float DeltaTime) override;

private:
	void StartSetSpringArmZOffsetTimeline();
	void StartSetArmMaterialParamTimeline();
	void BindTimelineFunctions();

	UFUNCTION(NetMulticast, UnReliable)
	void Multicast_StartCameraShake();

	UFUNCTION()
	void SetSpringArmZOffsetByFloatTimeline(float FloatCurveValue);

	UFUNCTION()
	void SetArmMaterialParamByFloatTimeline(float FloatCurveValue);

	void HandleOnHitSkyFallSlashSkill();
	const TArray<ANetherCrownEnemy*> GetSkillDetectedTargets() const;

	void CreateArmMaterialInstanceDynamic();

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

	UPROPERTY(EditDefaultsOnly)
	FName ArmMaterialScalarParameterName{ TEXT("SkyFallSlashAlpha") };

	FTimeline SpringArmZOffsetFloatTimeline{};
	FTimeline ArmMaterialFloatTimeline{};
};
