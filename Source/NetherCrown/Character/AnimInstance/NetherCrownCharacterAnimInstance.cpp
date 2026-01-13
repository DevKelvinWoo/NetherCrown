// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownCharacterAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownBasicAttackComponent.h"
#include "NetherCrown/Components/NetherCrownEquipComponent.h"
#include "NetherCrown/Components/NetherCrownSkillComponent.h"

void UNetherCrownCharacterAnimInstance::AnimNotify_ComboEnable()
{
	const ANetherCrownCharacter* OwningCharacter{ Cast<ANetherCrownCharacter>(GetOwningActor()) };
	UNetherCrownBasicAttackComponent* BasicAttackComponent{ OwningCharacter ? OwningCharacter->GetBasicAttackComponent() : nullptr};
	if (!(IsValid(BasicAttackComponent)))
	{
		return;
	}

	if (!OwningCharacter->HasAuthority())
	{
		return;
	}

	BasicAttackComponent->HandleEnableComboWindow();}

void UNetherCrownCharacterAnimInstance::AnimNotify_ComboDisable()
{
	const ANetherCrownCharacter* OwningCharacter{ Cast<ANetherCrownCharacter>(GetOwningActor()) };
	UNetherCrownBasicAttackComponent* BasicAttackComponent{ OwningCharacter ? OwningCharacter->GetBasicAttackComponent() : nullptr };
	if (!(IsValid(BasicAttackComponent)))
	{
		return;
	}

	BasicAttackComponent->HandleDisableComboWindow();
}

void UNetherCrownCharacterAnimInstance::AnimNotify_EquipStart()
{
	const ANetherCrownCharacter* OwningCharacter{ Cast<ANetherCrownCharacter>(GetOwningActor()) };
	UNetherCrownEquipComponent* EquipComponent{ OwningCharacter ? OwningCharacter->GetEquipComponent() : nullptr };
	if (!(IsValid(EquipComponent)))
	{
		return;
	}

	EquipComponent->NotifyEquipEndOrStart(false);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_EquipEnd()
{
	const ANetherCrownCharacter* OwningCharacter{ Cast<ANetherCrownCharacter>(GetOwningActor()) };
	UNetherCrownEquipComponent* EquipComponent{ OwningCharacter ? OwningCharacter->GetEquipComponent() : nullptr };
	if (!(IsValid(EquipComponent)))
	{
		return;
	}

	EquipComponent->NotifyEquipEndOrStart(true);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_HitTraceEnable()
{
	const ANetherCrownCharacter* OwningCharacter{ Cast<ANetherCrownCharacter>(GetOwningActor()) };
	UNetherCrownBasicAttackComponent* BasicAttackComponent{ OwningCharacter ? OwningCharacter->GetBasicAttackComponent() : nullptr };
	if (!(IsValid(BasicAttackComponent)))
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
	const ANetherCrownCharacter* OwningCharacter{ Cast<ANetherCrownCharacter>(GetOwningActor()) };
	UNetherCrownSkillComponent* SkillComponent{ OwningCharacter ? OwningCharacter->GetSkillComponent() : nullptr };
	if (!(IsValid(SkillComponent)))
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
	const ANetherCrownCharacter* OwningCharacter{ Cast<ANetherCrownCharacter>(GetOwningActor()) };
	UNetherCrownSkillComponent* SkillComponent{ OwningCharacter ? OwningCharacter->GetSkillComponent() : nullptr };
	if (!(IsValid(SkillComponent)))
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
	const ANetherCrownCharacter* OwningCharacter{ Cast<ANetherCrownCharacter>(GetOwningActor()) };
	UNetherCrownSkillComponent* SkillComponent{ OwningCharacter ? OwningCharacter->GetSkillComponent() : nullptr };
	if (!(IsValid(SkillComponent)))
	{
		return;
	}

	if (!OwningCharacter->HasAuthority())
	{
		return;
	}

	if (UNetherCrownBasicAttackComponent* BasicAttackComponent = OwningCharacter->GetBasicAttackComponent())
	{
		BasicAttackComponent->SetCanAttack(false);
	}

	SkillComponent->GetOnStopOrStartSkill().Broadcast(false);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_SkillEnd()
{
	const ANetherCrownCharacter* OwningCharacter{ Cast<ANetherCrownCharacter>(GetOwningActor()) };
	UNetherCrownSkillComponent* SkillComponent{ OwningCharacter ? OwningCharacter->GetSkillComponent() : nullptr };
	if (!(IsValid(SkillComponent)))
	{
		return;
	}

	if (!OwningCharacter->HasAuthority())
	{
		return;
	}

	if (UNetherCrownBasicAttackComponent* BasicAttackComponent = OwningCharacter->GetBasicAttackComponent())
	{
		BasicAttackComponent->SetCanAttack(true);
	}

	SkillComponent->GetOnStopOrStartSkill().Broadcast(true);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_SetCharacterMovementFly()
{
	const ANetherCrownCharacter* OwningCharacter{ Cast<ANetherCrownCharacter>(GetOwningActor()) };
	UCharacterMovementComponent* CharacterMovementComponent{ OwningCharacter ? OwningCharacter->GetCharacterMovement() : nullptr };
	if (!(IsValid(CharacterMovementComponent)))
	{
		return;
	}

	CharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Flying);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_SetCharacterMovementWalk()
{
	const ANetherCrownCharacter* OwningCharacter{ Cast<ANetherCrownCharacter>(GetOwningActor()) };
	UCharacterMovementComponent* CharacterMovementComponent{ OwningCharacter ? OwningCharacter->GetCharacterMovement() : nullptr };
	if (!(IsValid(CharacterMovementComponent)))
	{
		return;
	}

	CharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
}
