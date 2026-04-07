// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "NetherCrown/Data/NetherCrownSkillData.h"
#include "NetherCrownFrozenTempest.generated.h"

class UCurveVector;

UCLASS(Blueprintable)
class NETHERCROWN_API UNetherCrownFrozenTempest : public UNetherCrownSkillObject
{
	GENERATED_BODY()

public:
	UNetherCrownFrozenTempest();

protected:
	virtual void InitSkillObject() override;

	virtual void ExecuteSkillGameplay() override;
	virtual void PlaySkillCosmetics() override;

	virtual void TickFloatTimeline(float DeltaTime) override;

private:
	void CacheFrozenTempestData();

	void BindTimelineFunctions();

	void StartSetSkillCameraZoomTimeline();
	void StartSetCharacterOverlayStartMaterialTimeline();
	void StartSetCharacterOverlayEndMaterialTimeline();

	UFUNCTION(NetMulticast, UnReliable)
	void Multicast_StartFrozenTempestHitCosmetics();

	UFUNCTION(NetMulticast, UnReliable)
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

	void SetupFrozenTempestHitTimers();

	UPROPERTY(Transient)
	FNetherCrownFrozenTempestData FrozenTempestData{};

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> CachedCharacterOverlayMaterialStartCurveFloat{};

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> CachedCharacterOverlayMaterialEndCurveFloat{};

	UPROPERTY(Transient)
	TObjectPtr<UCurveVector> CachedSkillCameraZoomCurveVector{};

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInterface> CachedFrozenTempestTargetOverlayMaterial{};

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> CachedDynamicFrozenTempestMaterial{};

	FTimeline SkillCameraZoomVectorTimeline{};
	FTimeline CharacterOverlayStartMaterialFloatTimeline{};
	FTimeline CharacterOverlayEndMaterialFloatTimeline{};
};
