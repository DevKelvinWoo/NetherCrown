// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "NetherCrown/Data/NetherCrownSkillData.h"
#include "NetherCrownSkillDashAttack.generated.h"

class UNiagaraSystem;
class UCurveVector;

UCLASS(Blueprintable)
class NETHERCROWN_API UNetherCrownSkillDashAttack : public UNetherCrownSkillObject
{
	GENERATED_BODY()

public:
	UNetherCrownSkillDashAttack();

protected:
	virtual void PlaySkillCosmetics() override;
	virtual void ExecuteSkillGameplay() override;

	virtual void InitSkillObject() override;

	virtual void TickFloatTimeline(float DeltaTime) override;

private:
	void CacheDashAttackData();

	TArray<AActor*> DetectDashAttackTargets() const;

	void StartDashAttackTimer();
	void DashAttackToTargets();

	void LastDashAttack();

	void CacheDashAttackCosmeticData();

	void BindTimelineFunctions();

	UFUNCTION(Client, Reliable)
	void Client_StartDashAttackCameraPosBeginTimeline();

	UFUNCTION(Client, Reliable)
	void Client_StartDashAttackCameraPosEndTimeline();

	UFUNCTION(Client, UnReliable)
	void Client_ApplyPostProcess();

	UFUNCTION()
	void ApplyDashAttackCameraPos(const FVector& VectorCurveValue);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetOwnerCharacterRotToTarget(const FRotator& InTargetRot);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DashOwnerCharacter(const FVector StartLoc, const FVector TargetVec);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetCharacterCapsuleCollisionData(const bool bStartDashAttack);

	UFUNCTION(Client, UnReliable)
	void Client_StartPostProcessBlendEndTimer();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DeactivateDashAttackGhostTrail();

	UFUNCTION(Client, UnReliable)
	void Client_ActiveSkillHitCameraShake() const;

	UFUNCTION(Client, UnReliable)
	void Client_SetCameraViewLastDashAttack();

	void SetAttackLastDashAttackTimer();
	void AttackLastDashAttack();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayLastDashAttackMontage() const;

	void PlayDashAttackMontage(UAnimMontage* SkillAnimMontage) const;

	void HitDashAttack();
	void ApplyDashAttackDamageAndCrowdControl(ANetherCrownEnemy* TargetEnemy);

	void ClearDashAttackData();

	FTimerHandle DashAttackTimerHandle{};
	FTimerHandle DashAttackHitTimerHandle{};

	UPROPERTY(Transient)
	FNetherCrownDashAttackData DashAttackData{};

	int32 CurrentTargetIndex{ 0 };

	UPROPERTY(Transient)
	TArray<AActor*> CachedTargetActors{};

	UPROPERTY(Transient)
	TObjectPtr<AActor> CurrentTargetActor{};

	UPROPERTY(Transient)
	TObjectPtr<UNiagaraSystem> GhostTrailNiagaraSystem{};

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CachedLastDashAttackAnimMontage{};

	UPROPERTY(Transient)
	TObjectPtr<UCurveVector> CachedDashAttackCameraPosBeginCurve{};

	UPROPERTY(Transient)
	TObjectPtr<UCurveVector> CachedDashAttackCameraPosEndCurve{};

	FTimerHandle LastDashAttackCameraTimerHandle{};
	FTimerHandle LastDashAttackHitStartTimerHandle{};

	FTimeline DashAttackCameraPosBeginTimeline{};
	FTimeline DashAttackCameraPosEndTimeline{};
};
