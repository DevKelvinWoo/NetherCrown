// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NetherCrownCharacterAnimInstance.generated.h"

class ANetherCrownCharacter;
class UNetherCrownBasicAttackComponent;
class UNetherCrownEquipComponent;
class UNetherCrownSkillComponent;
class UCharacterMovementComponent;

UCLASS()
class NETHERCROWN_API UNetherCrownCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Character")
	TObjectPtr<ANetherCrownCharacter> CachedOwningCharacter;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Component")
	TObjectPtr<UNetherCrownBasicAttackComponent> CachedBasicAttackComponent;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Component")
	TObjectPtr<UNetherCrownEquipComponent> CachedEquipComponent;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Component")
	TObjectPtr<UNetherCrownSkillComponent> CachedSkillComponent;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Component")
	TObjectPtr<UCharacterMovementComponent> CachedCharacterMovementComponent;

private:
	void CacheInitData();
};
