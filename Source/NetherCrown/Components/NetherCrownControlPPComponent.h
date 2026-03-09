// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "NetherCrownControlPPComponent.generated.h"

class ANetherCrownCharacter;
class UPostProcessComponent;

UENUM()
enum class ENetherCrownPPType : uint8
{
	Default,
	Frozen,
	Charging,
	Lightning,
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownControlPPComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownControlPPComponent();

	void SetHandlingPostProcessComponent(UPostProcessComponent* PostProcessComponent);

	void ApplyPostProcess(const ENetherCrownPPType PPType, float Duration, const bool bEndTimerAutomatic = true);
	void StartSetPostProcessBlendEndTimeline();
	void StartSetPostProcessBlendStartTimeline();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void BindTimelineFunctions();

	void StartClearPostProcessTimer(float Duration);
	void ResetPostProcess();
	void ClearPostProcessImmediately();

	UFUNCTION()
	void SetPostProcessBlendStartByFloatTimeline(float FloatCurveValue);

	UFUNCTION()
	void SetPostProcessBlendEndByFloatTimeline(float FloatCurveValue);

	UFUNCTION()
	void HandlePostProcessBlendEndTimelineFinished();

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TMap<ENetherCrownPPType, FPostProcessSettings> PostProcessSettingsMap{};

	UPROPERTY(EditDefaultsOnly, Category = "Curve")
	TSoftObjectPtr<UCurveFloat> PostProcessStartCurveFloatSoft{};
	UPROPERTY(EditDefaultsOnly, Category = "Curve")
	TSoftObjectPtr<UCurveFloat> PostProcessEndCurveFloatSoft{};

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> CachedPostProcessStartCurveFloat{};
	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> CachedPostProcessEndCurveFloat{};
	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownCharacter> CachedCharacter{};

	TWeakObjectPtr<UPostProcessComponent> HandledPostProcessComponentWeak{};

	FTimeline PostProcessBlendStartFloatTimeline{};
	FTimeline PostProcessBlendEndFloatTimeline{};
};
