// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownSkillObject.h"
#include "Components/TimelineComponent.h"
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

	virtual void TickFloatTimeline(float DeltaTime) override;

private:
	void BindTimelineFunctions();

	void StartSetSkillCameraZoomTimeline();
	void StartSetCharacterOverlayStartMaterialTimeline();
	void StartSetCharacterOverlayEndMaterialTimeline();

	UFUNCTION(NetMulticast, UnReliable)
	void Multicast_StartFrozenTempestHitCosmetics();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetDetectedEnemyOverlayMaterial();

	UFUNCTION()
	void SetSkillCameraZoomByVectorTimeline(FVector VectorCurveValue);

	UFUNCTION()
	void SetCharacterOverlayStartMaterialByFloatTimeline(float FloatCurveValue);

	UFUNCTION()
	void SetCharacterOverlayEndMaterialByFloatTimeline(float FloatCurveValue);

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

	FTimeline SkillCameraZoomVectorTimeline{};
	FTimeline CharacterOverlayStartMaterialFloatTimeline{};
	FTimeline CharacterOverlayEndMaterialFloatTimeline{};
};
