// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownSkillObject.h"
#include "NetherCrownFrozenTempest.generated.h"

class UCurveVector;

UCLASS(Blueprintable)
class NETHERCROWN_API UNetherCrownFrozenTempest : public UNetherCrownSkillObject
{
	GENERATED_BODY()

protected:
	virtual void InitSkillObject() override;

	virtual void ExecuteSkillGameplay() override;
	virtual void PlaySkillCosmetics() override;

private:
	void StartSkillCameraZoomCurveTimer();
	void ApplySkillCameraZoomCurveFloat();

	void HandleOnHitFrozenTempestSkill();

	void PlayChargeCameraShake();

	const TArray<ANetherCrownEnemy*> GetSkillDetectedTargets() const;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UCurveVector> SkillCameraZoomCurveVectorSoft{};

	UPROPERTY()
	TObjectPtr<UCurveVector> CachedSkillCameraZoomCurveVector{};

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCameraShakeBase> SkillCameraShakeBaseClass{};

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCameraShakeBase> SKillChargeCameraShakeBaseClass{};

	FTimerHandle SkillCameraZoomCurveTimerHandle{};
	float SkillCameraZoomCurveElapsedTime{ 0.f };

	UPROPERTY(EditDefaultsOnly)
	float SkillDetectingSphereRadius{ 500.f };

	UPROPERTY(EditDefaultsOnly)
	float SkillDuration{ 3.f };

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UMaterialInterface> FrozenTempestTargetOverlayMaterialSoft{};

	UPROPERTY()
	TObjectPtr<UMaterialInterface> CachedFrozenTempestTargetOverlayMaterial{};
};
