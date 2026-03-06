// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownCharacterAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "NetherCrown/NetherCrown.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownBasicAttackComponent.h"
#include "NetherCrown/Components/NetherCrownEquipComponent.h"
#include "NetherCrown/Components/NetherCrownSkillComponent.h"
#include "NetherCrown/Weapon/NetherCrownWeapon.h"

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

void UNetherCrownCharacterAnimInstance::AnimNotify_EquipStart()
{
	if (!(IsValid(CachedOwningCharacter)) || !(IsValid(CachedEquipComponent)))
	{
		UE_LOG(LogNetherCrown, Error, TEXT("CachedOwningCharacter or CachedEquipComponent is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	CachedEquipComponent->NotifyEquipEndOrStart(false);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_EquipEnd()
{
	if (!(IsValid(CachedOwningCharacter)) || !(IsValid(CachedEquipComponent)))
	{
		UE_LOG(LogNetherCrown, Error, TEXT("CachedOwningCharacter or CachedEquipComponent is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	CachedEquipComponent->NotifyEquipEndOrStart(true);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_SkillStart()
{
	if (!(IsValid(CachedOwningCharacter)) || !(IsValid(CachedSkillComponent)) || !(IsValid(CachedBasicAttackComponent)))
	{
		UE_LOG(LogNetherCrown, Error, TEXT("CachedOwningCharacter or CachedSkillComponent or CachedBasicAttackComponent is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	CachedBasicAttackComponent->SetCanAttack(false);
	CachedSkillComponent->GetOnStopOrStartSkill().Broadcast(false);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_SkillEnd()
{
	if (!(IsValid(CachedOwningCharacter)) || !(IsValid(CachedSkillComponent)) || !(IsValid(CachedBasicAttackComponent)))
	{
		UE_LOG(LogNetherCrown, Error, TEXT("CachedOwningCharacter or CachedSkillComponent is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	CachedBasicAttackComponent->SetCanAttack(true);
	CachedSkillComponent->GetOnStopOrStartSkill().Broadcast(true);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_SetCharacterMovementFly()
{
	if (!(IsValid(CachedOwningCharacter)) || !(IsValid(CachedCharacterMovementComponent)))
	{
		UE_LOG(LogNetherCrown, Error, TEXT("CachedOwningCharacter or CachedCharacterMovementComponent is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	CachedCharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Flying);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_SetCharacterMovementWalk()
{
	if (!(IsValid(CachedOwningCharacter)) || !(IsValid(CachedCharacterMovementComponent)))
	{
		UE_LOG(LogNetherCrown, Error, TEXT("CachedOwningCharacter or CachedCharacterMovementComponent is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	CachedCharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
}
