// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownCharacterAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownBasicAttackComponent.h"
#include "NetherCrown/Components/NetherCrownEquipComponent.h"
#include "NetherCrown/Components/NetherCrownSkillComponent.h"

void UNetherCrownCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningCharacter = Cast<ANetherCrownCharacter>(GetOwningActor());
	if (!IsValid(OwningCharacter))
	{
		return;
	}

	BasicAttackComponent = OwningCharacter->GetBasicAttackComponent();
	EquipComponent = OwningCharacter->GetEquipComponent();
	SkillComponent = OwningCharacter->GetSkillComponent();
	CharacterMovementComponent = OwningCharacter->GetCharacterMovement();
}

void UNetherCrownCharacterAnimInstance::AnimNotify_ComboEnable()
{
	if (!(IsValid(OwningCharacter)) || !(IsValid(BasicAttackComponent)))
	{
		return;
	}

	if (!OwningCharacter->HasAuthority())
	{
		return;
	}

	BasicAttackComponent->HandleEnableComboWindow();
}

void UNetherCrownCharacterAnimInstance::AnimNotify_ComboDisable()
{
	if (!(IsValid(OwningCharacter)) || !(IsValid(BasicAttackComponent)))
	{
		return;
	}

	BasicAttackComponent->HandleDisableComboWindow();
}

void UNetherCrownCharacterAnimInstance::AnimNotify_EquipStart()
{
	if (!(IsValid(OwningCharacter)) || !(IsValid(EquipComponent)))
	{
		return;
	}

	EquipComponent->NotifyEquipEndOrStart(false);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_EquipEnd()
{
	if (!(IsValid(OwningCharacter)) || !(IsValid(EquipComponent)))
	{
		return;
	}

	EquipComponent->NotifyEquipEndOrStart(true);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_HitTraceEnable()
{
	if (!(IsValid(OwningCharacter)) || !(IsValid(BasicAttackComponent)))
	{
		return;
	}

	if (!OwningCharacter->IsLocallyControlled())
	{
		return;
	}

	BasicAttackComponent->HandleEnableHitTrace();
}

void UNetherCrownCharacterAnimInstance::AnimNotify_SkillSlowBegin()
{
	if (!(IsValid(OwningCharacter)) || !(IsValid(SkillComponent)))
	{
		return;
	}

	if (OwningCharacter->HasAuthority())
	{
		return;
	}

	SkillComponent->SetActiveSkillSlowPlayRate(true);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_SkillSlowEnd()
{
	if (!(IsValid(OwningCharacter)) || !(IsValid(SkillComponent)))
	{
		return;
	}

	if (OwningCharacter->HasAuthority())
	{
		return;
	}

	SkillComponent->SetActiveSkillSlowPlayRate(false);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_SkillStart()
{
	if (!(IsValid(OwningCharacter)) || !(IsValid(SkillComponent)))
	{
		return;
	}

	if (!OwningCharacter->HasAuthority())
	{
		return;
	}

	if (IsValid(BasicAttackComponent))
	{
		BasicAttackComponent->SetCanAttack(false);
	}

	SkillComponent->GetOnStopOrStartSkill().Broadcast(false);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_SkillEnd()
{
	if (!(IsValid(OwningCharacter)) || !(IsValid(SkillComponent)))
	{
		return;
	}

	if (!OwningCharacter->HasAuthority())
	{
		return;
	}

	if (IsValid(BasicAttackComponent))
	{
		BasicAttackComponent->SetCanAttack(true);
	}

	SkillComponent->GetOnStopOrStartSkill().Broadcast(true);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_SetCharacterMovementFly()
{
	if (!(IsValid(OwningCharacter)) || !(IsValid(CharacterMovementComponent)))
	{
		return;
	}

	CharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Flying);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_SetCharacterMovementWalk()
{
	if (!(IsValid(OwningCharacter)) || !(IsValid(CharacterMovementComponent)))
	{
		return;
	}

	CharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
}
