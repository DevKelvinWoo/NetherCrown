// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownCharacterAnimInstance.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownBasicAttackComponent.h"
#include "NetherCrown/Components/NetherCrownEquipComponent.h"
#include "NetherCrown/Components/NetherCrownSkillComponent.h"

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

	BasicAttackComponent->HandleEnableComboWindow();}

void UNetherCrownCharacterAnimInstance::AnimNotify_ComboDisable()
{
	ANetherCrownCharacter* OwningCharacter{ Cast<ANetherCrownCharacter>(GetOwningActor()) };
	UNetherCrownBasicAttackComponent* BasicAttackComponent{ OwningCharacter ? OwningCharacter->GetBasicAttackComponent() : nullptr };
	if (!(IsValid(BasicAttackComponent)))
	{
		return;
	}

	BasicAttackComponent->HandleDisableComboWindow();
}

void UNetherCrownCharacterAnimInstance::AnimNotify_EquipStart()
{
	ANetherCrownCharacter* OwningCharacter{ Cast<ANetherCrownCharacter>(GetOwningActor()) };
	UNetherCrownEquipComponent* EquipComponent{ OwningCharacter ? OwningCharacter->GetEquipComponent() : nullptr };
	if (!(IsValid(EquipComponent)))
	{
		return;
	}

	EquipComponent->NotifyEquipEndOrStart(false);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_EquipEnd()
{
	ANetherCrownCharacter* OwningCharacter{ Cast<ANetherCrownCharacter>(GetOwningActor()) };
	UNetherCrownEquipComponent* EquipComponent{ OwningCharacter ? OwningCharacter->GetEquipComponent() : nullptr };
	if (!(IsValid(EquipComponent)))
	{
		return;
	}

	EquipComponent->NotifyEquipEndOrStart(true);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_HitTraceEnable()
{
	ANetherCrownCharacter* OwningCharacter{ Cast<ANetherCrownCharacter>(GetOwningActor()) };
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
	ANetherCrownCharacter* OwningCharacter{ Cast<ANetherCrownCharacter>(GetOwningActor()) };
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
	ANetherCrownCharacter* OwningCharacter{ Cast<ANetherCrownCharacter>(GetOwningActor()) };
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
