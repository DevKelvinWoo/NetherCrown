// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownDodgeSkill.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

UNetherCrownDodgeSkill::UNetherCrownDodgeSkill()
{
	SetSkillTag(NetherCrownTags::Skill_DodgeSkill);
}

void UNetherCrownDodgeSkill::InitSkillObject()
{
	Super::InitSkillObject();

	CacheDodgeSkillData();
}

void UNetherCrownDodgeSkill::ExecuteSkillGameplay()
{
	Super::ExecuteSkillGameplay();

	DodgeCharacter();
}

void UNetherCrownDodgeSkill::PlaySkillCosmetics()
{
	Super::PlaySkillCosmetics();
}

void UNetherCrownDodgeSkill::CacheDodgeSkillData()
{
	const UNetherCrownSkillDataAsset* SkillDataAsset{ FNetherCrownUtilManager::GetSkillDataAssetByGameplayTag(NetherCrownTags::Skill_DodgeSkill) };
	const UNetherCrownDodgeSkillDataAsset* DodgeSkillDataAsset{ Cast<UNetherCrownDodgeSkillDataAsset>(SkillDataAsset) };
	if (!ensureAlways(IsValid(DodgeSkillDataAsset)))
	{
		return;
	}

	DodgeSkillData = DodgeSkillDataAsset->GetDodgeSkillData();
}

void UNetherCrownDodgeSkill::DodgeCharacter()
{
	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || !SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	FVector DodgeDirection{ SkillOwnerCharacter->GetLastMoveDirection().GetSafeNormal2D() };
	if (DodgeDirection.IsNearlyZero())
	{
		DodgeDirection = SkillOwnerCharacter->GetActorForwardVector().GetSafeNormal2D();
	}

	const float DodgeDistance{ DodgeSkillData.DodgeDistance };
	SkillOwnerCharacter->LaunchCharacter(DodgeDirection * DodgeDistance, true, false);
}
