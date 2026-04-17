// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownCrowdControlTypes.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "NetherCrown/Data/NetherCrownCrowdControlCosmeticData.h"
#include "NetherCrownCrowdControlComponent.generated.h"

class UAnimMontage;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownCrowdControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownCrowdControlComponent();

	ENetherCrownCrowdControlType GetCrowdControlType() const { return CrowdControlType; }
	bool IsCrowdControlActive(const ENetherCrownCrowdControlType InCrowdControlType) const;
	void ApplyCrowdControl(const ENetherCrownCrowdControlType InCrowdControlType, float DurationTime);

	//@NOTE : CC Implements
	void KnockBack(const FVector& KnockBackVector) const;
	void Frozen() const;
	void Stun() const;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void InitLoadData();
	void LoadCrowdControlCosmeticData();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetCrowdControlState(const ENetherCrownCrowdControlType InCrowdControlType);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ClearCrowdControl_Cosmetics(const ENetherCrownCrowdControlType InCrowdControlType);

	void ClearCrowdControl(const ENetherCrownCrowdControlType InCrowdControlType);

	void RefreshCrowdControlType();
	void RefreshMovementAndAnimationSettings() const;
	void SetCrowdControlActive(const ENetherCrownCrowdControlType InCrowdControlType, const bool bActive);
	FTimerHandle* GetCrowdControlTimerHandle(const ENetherCrownCrowdControlType InCrowdControlType);
	ENetherCrownCrowdControlType GetHighestPriorityCrowdControlType() const;
	int32 GetCrowdControlPriority(const ENetherCrownCrowdControlType InCrowdControlType) const;

	void ClearFrozenCosmetics();
	void ClearStunCosmetics();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetActiveStatusNiagaraSystem(const bool bEnable, const ENetherCrownCrowdControlType InCrowdControlType = ENetherCrownCrowdControlType::NONE) const;

	void BindTimelineFunctions();

	void StartSetFrozenTargetOverlayEndMaterialTimeline();

	UFUNCTION()
	void SetFrozenTargetOverlayEndMaterialByFloatTimeline(float FloatCurveValue);

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> CachedFrozenTargetOverlayMaterialEndCurveFloat{};

	FTimeline FrozenTargetOverlayEndMaterialFloatTimeline{};

	UPROPERTY(Transient)
	FNetherCrownCrowdControlCosmeticData CrowdControlCosmeticData{};

	UPROPERTY(EditDefaultsOnly, Category = "CrowdControlCosmeticDataAsset")
	TSoftObjectPtr<UNetherCrownCrowdControlCosmeticDataAsset> CrowdControlCosmeticDataAssetSoft{};

	UPROPERTY(Replicated)
	ENetherCrownCrowdControlType CrowdControlType{ ENetherCrownCrowdControlType::NONE };

	uint8 ActiveCrowdControlFlags{};

	FTimerHandle KnockBackCrowdControlTimerHandle{};
	FTimerHandle FrozenCrowdControlTimerHandle{};
	FTimerHandle StunCrowdControlTimerHandle{};

	UPROPERTY(Transient)
	TObjectPtr<ACharacter> CachedOwner{};
};
