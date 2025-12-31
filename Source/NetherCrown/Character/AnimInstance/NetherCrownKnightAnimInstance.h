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

public:
	UFUNCTION()
	void AnimNotify_HitSkyFallSlashSkill();

	FOnHitSkyFallSlashSkill& GetOnHitSkyFallSlashSkill() { return OnHitSkyFallSlashSkill; }

private:
	FOnHitSkyFallSlashSkill OnHitSkyFallSlashSkill;
};
