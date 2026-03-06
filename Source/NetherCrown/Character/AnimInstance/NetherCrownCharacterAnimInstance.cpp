// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownCharacterAnimInstance.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"

void UNetherCrownCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	CacheInitData();
}

void UNetherCrownCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	CacheInitData();
}

void UNetherCrownCharacterAnimInstance::CacheInitData()
{
	CachedOwningCharacter = Cast<ANetherCrownCharacter>(GetOwningActor());

	if (IsValid(CachedOwningCharacter))
	{
		CachedBasicAttackComponent = CachedOwningCharacter->GetBasicAttackComponent();
		CachedEquipComponent = CachedOwningCharacter->GetEquipComponent();
		CachedSkillComponent = CachedOwningCharacter->GetSkillComponent();
		CachedCharacterMovementComponent = CachedOwningCharacter->GetCharacterMovement();
	}
}
