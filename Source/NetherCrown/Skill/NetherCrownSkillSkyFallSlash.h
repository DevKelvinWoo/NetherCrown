// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "NetherCrown/Data/NetherCrownSkillData.h"
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
	void CacheSkyFallSlashData();

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

	UPROPERTY(Transient)
	FNetherCrownSkyFallSlashData SkyFallSlashData{};

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> CachedSkillCameraCurveFloat{};

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> CachedSkillArmMaterialCurveFloat{};

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> ArmMaterialInstanceDynamic{};

	FTimeline SpringArmZOffsetFloatTimeline{};
	FTimeline ArmMaterialFloatTimeline{};
};
