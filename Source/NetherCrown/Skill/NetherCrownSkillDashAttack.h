// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownSkillObject.h"
#include "NetherCrownSkillDashAttack.generated.h"

class UNiagaraSystem;

UCLASS(Blueprintable)
class NETHERCROWN_API UNetherCrownSkillDashAttack : public UNetherCrownSkillObject
{
	GENERATED_BODY()

protected:
	virtual void PlaySkillCosmetics() override;
	virtual void ExecuteSkillGameplay() override;

	virtual void InitSkillObject() override;

private:
	TArray<AActor*> DetectDashAttackTargets() const;

	void StartDashAttackTimer();
	void DashAttackToTargets();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetOwnerCharacterRotToTarget(const FRotator& InTargetRot);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DashOwnerCharacter(const FVector StartLoc, const FVector TargetVec);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetCharacterCapsuleCollisionData(const bool bStartDashAttack);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DeactivateDashAttackGhostTrail();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ActiveSkillHitCameraShake() const;

	void PlayLoopDashAttackMontage() const;

	void HitDashAttack() const;

	void ClearDashAttackData();

	FTimerHandle DashAttackTimerHandle{};
	FTimerHandle DashAttackHitTimerHandle{};

	UPROPERTY(EditDefaultsOnly)
	float SkillDetectSphereOffset{ 750.f };

	UPROPERTY(EditDefaultsOnly)
	float EndLocationOffset{ 150.f };

	UPROPERTY(EditDefaultsOnly)
	float DashTimerRate{ 0.4f };

	UPROPERTY(EditDefaultsOnly)
	float DashDuration{ 0.2f };

	UPROPERTY(EditDefaultsOnly)
	int32 MaxTargetNum{ 5 };

	int32 CurrentTargetIndex{ 0 };

	UPROPERTY()
	TArray<AActor*> CachedTargetActors{};

	UPROPERTY()
	TObjectPtr<AActor> CurrentTargetActor{};

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UNiagaraSystem> GhostTrailNiagaraSystemSoft{};

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> GhostTrailNiagaraSystem{};

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCameraShakeBase> DashAttackHitCameraShakeClass{};
};
