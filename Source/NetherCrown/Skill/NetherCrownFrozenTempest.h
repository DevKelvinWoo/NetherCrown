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
	void ApplySkillCameraZoomCurveVector();
	void SetSkillCameraSpringArmValues();

	void StartCharacterOverlayMaterialStartTimer();
	void ApplyCharacterOverlayStartMaterial();
	void SetStartCharacterOverlayMaterialScalarParam();

	void StartCharacterOverlayMaterialEndTimer();
	void ApplyCharacterOverlayEndMaterial();
	void SetEndCharacterOverlayMaterialScalarParam();

	void HandleOnHitFrozenTempestSkill();

	void PlayChargeCameraShake();

	const TArray<ANetherCrownEnemy*> GetSkillDetectedTargets() const;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UCurveVector> SkillCameraZoomCurveVectorSoft{};

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UCurveFloat> CharacterOverlayMaterialStartCurveFloatSoft{};

	UPROPERTY()
	TObjectPtr<UCurveFloat> CachedCharacterOverlayMaterialStartCurveFloat{};

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UCurveFloat> CharacterOverlayMaterialEndCurveFloatSoft{};

	UPROPERTY()
	TObjectPtr<UCurveFloat> CachedCharacterOverlayMaterialEndCurveFloat{};

	UPROPERTY()
	TObjectPtr<UCurveVector> CachedSkillCameraZoomCurveVector{};

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCameraShakeBase> SkillCameraShakeBaseClass{};

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCameraShakeBase> SkillChargeCameraShakeBaseClass{};

	FTimerHandle SkillCameraZoomCurveTimerHandle{};
	float SkillCameraZoomCurveElapsedTime{ 0.f };

	FTimerHandle CharacterOverlayMaterialStartTimerHandle{};
	FTimerHandle CharacterOverlayMaterialEndTimerHandle{};
	float CharacterOverlayMaterialElapsedTime{ 0.f };

	UPROPERTY(EditDefaultsOnly)
	float SkillDetectingSphereRadius{ 500.f };

	UPROPERTY(EditDefaultsOnly)
	float FrozenDuration{ 3.f };

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UMaterialInterface> FrozenTempestTargetOverlayMaterialSoft{};

	UPROPERTY()
	TObjectPtr<UMaterialInterface> CachedFrozenTempestTargetOverlayMaterial{};

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CachedDynamicFrozenTempestMaterial{};
};
