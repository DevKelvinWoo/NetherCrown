// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownCharacterAnimInstance.h"
#include "NetherCrownKnightAnimInstance.generated.h"

UCLASS()
class NETHERCROWN_API UNetherCrownKnightAnimInstance : public UNetherCrownCharacterAnimInstance
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE(FOnHitFrozenTempestSkill);

public:
	UFUNCTION()
	void AnimNotify_HitFrozenTempestSkill();

	FOnHitFrozenTempestSkill& GetOnHitFrozenTempestSkill() { return OnHitFrozenTempestSkill; }

private:
	FOnHitFrozenTempestSkill OnHitFrozenTempestSkill;
};
