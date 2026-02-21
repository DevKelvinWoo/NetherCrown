// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownCharacterAnimInstance.h"
#include "NetherCrownKnightAnimInstance.generated.h"

UCLASS()
class NETHERCROWN_API UNetherCrownKnightAnimInstance : public UNetherCrownCharacterAnimInstance
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE(FOnHitSkyFallSlashSkill);
	DECLARE_MULTICAST_DELEGATE(FOnHitFrozenTempestSkill);

public:
	UFUNCTION()
	void AnimNotify_HitSkyFallSlashSkill();

	UFUNCTION()
	void AnimNotify_HitFrozenTempestSkill();

	FOnHitSkyFallSlashSkill& GetOnHitSkyFallSlashSkill() { return OnHitSkyFallSlashSkill; }
	FOnHitFrozenTempestSkill& GetOnHitFrozenTempestSkill() { return OnHitFrozenTempestSkill; }

private:
	FOnHitSkyFallSlashSkill OnHitSkyFallSlashSkill;
	FOnHitFrozenTempestSkill OnHitFrozenTempestSkill;
};
