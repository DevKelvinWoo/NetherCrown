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
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownControlPPComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownControlPPComponent();

	void SetHandlingPostProcessComponent(UPostProcessComponent* PostProcessComponent);

	void ApplyPostProcess(const ENetherCrownPPType PPType, float Duration);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void StartClearPostProcessTimer(float Duration);
	void ClearPostProcess();

	void StartPostProcessBlendStartTimer();
	void ApplyPostProcessBlendStartFloat();
	void StartPostProcessBlendEndTimer();
	void ApplyPostProcessBlendEndFloat();

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
