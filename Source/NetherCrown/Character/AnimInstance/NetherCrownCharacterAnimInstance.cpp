// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownCharacterAnimInstance.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownBasicAttackComponent.h"

void UNetherCrownCharacterAnimInstance::AnimNotify_ComboEnable()
{
	ANetherCrownCharacter* OwningCharacter{ Cast<ANetherCrownCharacter>(GetOwningActor()) };
	UNetherCrownBasicAttackComponent* BasicAttackComponent{ OwningCharacter ? OwningCharacter->GetBasicAttackComponent() : nullptr};
	if (!(IsValid(BasicAttackComponent)))
	{
		return;
	}

	if (!OwningCharacter->HasAuthority())
	{
		return;
	}

	BasicAttackComponent->EnableComboWindow();}

void UNetherCrownCharacterAnimInstance::AnimNotify_ComboDisable()
{
	ANetherCrownCharacter* OwningCharacter{ Cast<ANetherCrownCharacter>(GetOwningActor()) };
	UNetherCrownBasicAttackComponent* BasicAttackComponent{ OwningCharacter ? OwningCharacter->GetBasicAttackComponent() : nullptr };
	if (!(IsValid(BasicAttackComponent)))
	{
		return;
	}

	if (!OwningCharacter->HasAuthority())
	{
		return;
	}

	BasicAttackComponent->DisableComboAndPlayQueuedComboWindow();
}
