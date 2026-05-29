// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "NetherCrownPPTypes.h"
#include "NetherCrown/Data/NetherCrownPostProcessCosmeticData.h"
#include "NetherCrownControlPPComponent.generated.h"

class ANetherCrownCharacter;
class UCameraComponent;
class UPostProcessComponent;
class UCurveFloat;
class UNetherCrownPlayerStatComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownControlPPComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownControlPPComponent();

	void SetHandlingPostProcessComponent(UPostProcessComponent* PostProcessComponent);
	void SetHandlingCameraComponent(UCameraComponent* CameraComponent);

	void ApplyPostProcess(const ENetherCrownPPType PPType, const float Duration, const bool bEndTimerAutomatic = true);
	void TryBindLowHealthPostProcess();
	void FlashCrowdControlPostProcess();
	void StartSetPostProcessBlendEndTimeline();
	void StartSetPostProcessBlendStartTimeline();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(const float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void LoadPostProcessCosmeticData();
	void BindTimelineFunctions();

	void StartClearPostProcessTimer(const float Duration);
	void ResetPostProcess();
	void ClearPostProcessImmediately();
	void HandleCharacterHPModified(const float RemainHPRatio);
	void SetLowHealthCameraPostProcessActive(const bool bInActive);
	void RestorePostProcessAfterCrowdControlFlash();

	UFUNCTION()
	void SetPostProcessBlendStartByFloatTimeline(float FloatCurveValue);

	UFUNCTION()
	void SetPostProcessBlendEndByFloatTimeline(float FloatCurveValue);

	UFUNCTION()
	void HandlePostProcessBlendEndTimelineFinished();

	UPROPERTY(EditDefaultsOnly, Category = "PostProcessCosmeticDataAsset")
	TSoftObjectPtr<UNetherCrownPostProcessCosmeticDataAsset> PostProcessCosmeticDataAssetSoft{};

	UPROPERTY(Transient)
	FNetherCrownPostProcessCosmeticData PostProcessCosmeticData{};

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> CachedPostProcessStartCurveFloat{};

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> CachedPostProcessEndCurveFloat{};

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownCharacter> CachedCharacter{};

	TWeakObjectPtr<UPostProcessComponent> HandledPostProcessComponentWeak{};
	TWeakObjectPtr<UCameraComponent> HandledCameraComponentWeak{};

	UPROPERTY(Transient)
	TObjectPtr<UNetherCrownPlayerStatComponent> BoundPlayerStatComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "PostProcess")
	float LowHealthPostProcessThreshold{ 0.5f };

	UPROPERTY(EditDefaultsOnly, Category = "PostProcess")
	float CrowdControlPostProcessFlashDuration{ 1.f };

	FTimeline PostProcessBlendStartFloatTimeline{};
	FTimeline PostProcessBlendEndFloatTimeline{};
	FTimerHandle TimerHandle_CrowdControlPostProcessFlash;
};
