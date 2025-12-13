// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NetherCrownCharacterAnimInstance.generated.h"

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
	void AnimNotify_EquipStart();

	UFUNCTION()
	void AnimNotify_EquipEnd();
};
