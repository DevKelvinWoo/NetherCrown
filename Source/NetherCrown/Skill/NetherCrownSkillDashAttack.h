// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownSkillObject.h"
#include "Components/TimelineComponent.h"
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
	TArray<AActor*> DetectDashAttackTargets() const;

	void StartDashAttackTimer();
	void DashAttackToTargets();

	void LastDashAttack();

	void CacheDashAttackCosmeticData();

	void BindTimelineFunctions();

	void StartDashAttackCameraPosBeginTimeline();

	UFUNCTION(Client, Reliable)
	void Client_StartDashAttackCameraPosEndTimeline();

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

	UPROPERTY(EditDefaultsOnly, Category = "SkillData")
	float SkillDetectSphereOffset{ 750.f };

	UPROPERTY(EditDefaultsOnly, Category = "SkillData")
	float EndLocationOffset{ 50.f };

	UPROPERTY(EditDefaultsOnly, Category = "SkillData")
	float DashTimerRate{ 0.4f };

	UPROPERTY(EditDefaultsOnly, Category = "SkillData")
	float DashDuration{ 0.2f };

	UPROPERTY(EditDefaultsOnly, Category = "SkillData")
	int32 MaxTargetNum{ 5 };

	UPROPERTY(EditDefaultsOnly, Category = "SkillData")
	float StunDuration{ 3.f };

	UPROPERTY(EditDefaultsOnly, Category = "CameraData")
	float LastDashAttackCameraDistance{ 500.f };

	UPROPERTY(EditDefaultsOnly, Category = "NiagaraSystem")
	TSoftObjectPtr<UNiagaraSystem> GhostTrailNiagaraSystemSoft{};

	UPROPERTY(EditDefaultsOnly, Category = "CameraShakeClass")
	TSubclassOf<UCameraShakeBase> DashAttackHitCameraShakeClass{};

	UPROPERTY(EditDefaultsOnly, Category = "AnimMontage")
	TSoftObjectPtr<UAnimMontage> LastDashAttackAnimMontageSoft{};

	UPROPERTY(EditDefaultsOnly, Category = "CurveData")
	TSoftObjectPtr<UCurveVector> DashAttackCameraPosBeginCurveSoft{};

	UPROPERTY(EditDefaultsOnly, Category = "CurveData")
	TSoftObjectPtr<UCurveVector> DashAttackCameraPosEndCurveSoft{};

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
