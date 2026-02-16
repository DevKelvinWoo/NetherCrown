// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrownCrowdControlComponent.generated.h"

class UAnimMontage;

UENUM()
enum class ENetherCrownCrowdControlType : uint8
{
	NONE,
	KNOCK_BACK,
	FROZEN,
	STUN,
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownCrowdControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownCrowdControlComponent();

	ENetherCrownCrowdControlType GetCrowdControlType() const { return CrowdControlType; }
	void ApplyCrowdControl(const ENetherCrownCrowdControlType InCrowdControlType, float DurationTime);

	//@NOTE : CC Implements
	void KnockBack(const FVector& KnockBackVector) const;
	void Frozen() const;
	void Stun() const;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void InitLoadData();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayCrowdControlAnim(const ENetherCrownCrowdControlType InCrowdControlType);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ClearCrowdControl_Cosmetics();

	void ClearCrowdControl();

	void PlayCrowdControlAnim(const ENetherCrownCrowdControlType InCrowdControlType);

	void ResetMovementAndAnimationSettings() const;
	void ClearFrozenCosmetics();
	void ClearStunCosmetics();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetActiveStatusNiagaraSystem(const ENetherCrownCrowdControlType InCrowdControlType, const bool bEnable) const;

	FTimerHandle FrozenTargetOverlayMaterialEndTimerHandle{};
	void StartFrozenTargetOverlayMaterialEndTimer();
	void ApplyFrozenTargetOverlayEndMaterial();
	void SetEndFrozenTargetOverlayMaterialScalarParam();
	float FrozenTargetOverlayMaterialElapsedTime{ 0.f };

	UPROPERTY()
	TObjectPtr<UCurveFloat> CachedFrozenTargetOverlayMaterialEndCurveFloat{};

	FTimerHandle CrowdControlTimerHandle{};

	UPROPERTY(EditDefaultsOnly)
	TMap<ENetherCrownCrowdControlType, TSoftObjectPtr<UAnimMontage>> CrowdControlAnimMap{};

	UPROPERTY(Replicated)
	ENetherCrownCrowdControlType CrowdControlType{ ENetherCrownCrowdControlType::NONE };
};
