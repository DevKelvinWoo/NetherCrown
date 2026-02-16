// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrownControlPPComponent.generated.h"

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
	void StartPostProcessBlendEndTimer();

protected:
	virtual void BeginPlay() override;

private:
	void StartClearPostProcessTimer(float Duration);
	void ResetPostProcess();
	void ClearPostProcessImmediately();

	void StartPostProcessBlendStartTimer();
	void ApplyPostProcessBlendStartFloat();
	void ApplyPostProcessBlendEndFloat();

	void SetBeginPostProcessBlendWeight() const;
	void SetEndPostProcessBlendWeight() const;

	FTimerHandle PostProcessBlendStartTimerHandle{};
	FTimerHandle PostProcessBlendEndTimerHandle{};
	float PostProcessBlendElapsedTime{ 0.f };

	UPROPERTY(EditDefaultsOnly)
	TMap<ENetherCrownPPType, FPostProcessSettings> PostProcessSettingsMap{};

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UCurveFloat> PostProcessStartCurveFloatSoft{};
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UCurveFloat> PostProcessEndCurveFloatSoft{};

	UPROPERTY()
	TObjectPtr<UCurveFloat> CachedPostProcessStartCurveFloat{};
	UPROPERTY()
	TObjectPtr<UCurveFloat> CachedPostProcessEndCurveFloat{};

	TWeakObjectPtr<UPostProcessComponent> HandledPostProcessComponentWeak{};
};
