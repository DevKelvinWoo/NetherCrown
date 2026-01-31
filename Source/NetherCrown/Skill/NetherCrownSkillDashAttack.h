// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownSkillObject.h"
#include "NetherCrownSkillDashAttack.generated.h"

UCLASS(Blueprintable)
class NETHERCROWN_API UNetherCrownSkillDashAttack : public UNetherCrownSkillObject
{
	GENERATED_BODY()

protected:
	virtual void PlaySkillCosmetics() override;
	virtual void ExecuteSkillGameplay() override;

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

	void ClearDashAttackData();

	FTimerHandle DashAttackTimerHandle{};

	UPROPERTY(EditDefaultsOnly)
	float SkillDetectSphereOffset{ 750.f };

	UPROPERTY(EditDefaultsOnly)
	float EndLocationOffset{ 150.f };

	UPROPERTY(EditDefaultsOnly)
	float DashTimerRate{ 0.75f };

	UPROPERTY(EditDefaultsOnly)
	float DashDuration{ 0.15f };

	UPROPERTY(EditDefaultsOnly)
	int32 MaxTargetNum{ 5 };

	int32 CurrentTargetIndex{ 0 };

	UPROPERTY()
	TArray<AActor*> CachedTargetActors{};
};
