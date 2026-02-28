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

public:
	UFUNCTION()
	void AnimNotify_ComboEnable();

	UFUNCTION()
	void AnimNotify_ComboDisable();

	UFUNCTION()
	void AnimNotify_BasicAttackEnd();

	UFUNCTION()
	void AnimNotify_EquipStart();

	UFUNCTION()
	void AnimNotify_EquipEnd();

	UFUNCTION()
	void AnimNotify_HitTraceEnable();

	UFUNCTION()
	void AnimNotify_SkillSlowBegin();

	UFUNCTION()
	void AnimNotify_SkillSlowEnd();

	UFUNCTION()
	void AnimNotify_SkillStart();

	UFUNCTION()
	void AnimNotify_SkillEnd();

	UFUNCTION()
	void AnimNotify_SetCharacterMovementFly();

	UFUNCTION()
	void AnimNotify_SetCharacterMovementWalk();

	UFUNCTION()
	void AnimNotify_ActiveWeaponAuraNiagara();

	UFUNCTION()
	void AnimNotify_DeactiveWeaponAuraNiagara();

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
