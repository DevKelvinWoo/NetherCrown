// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownActionControlComponent.h"

#include "NetherCrownEquipComponent.h"
#include "NetherCrownSkillComponent.h"
#include "NetherCrownBasicAttackComponent.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"

UNetherCrownActionControlComponent::UNetherCrownActionControlComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UNetherCrownActionControlComponent::CanMove() const
{
	const UNetherCrownEquipComponent* EquipComponent{ EquipComponentWeak.Get() };
	const UNetherCrownBasicAttackComponent* BasicAttackComponent{ BasicAttackComponentWeak.Get() };
	const UNetherCrownSkillComponent* SkillComponent{ SkillComponentWeak.Get() };
	if (!ensureAlways(IsValid(EquipComponent)) || !ensureAlways(IsValid(BasicAttackComponent)) || !ensureAlways(IsValid(SkillComponent)))
	{
		return false;
	}

	if (EquipComponent->IsEquipping() || BasicAttackComponent->IsAttacking() || SkillComponent->IsUsingSkill())
	{
		return false;
	}

	return true;
}

bool UNetherCrownActionControlComponent::CanAttack() const
{
	const UNetherCrownSkillComponent* SkillComponent{ SkillComponentWeak.Get() };
	if (!ensureAlways(IsValid(SkillComponent)) || !ensureAlways(IsValid(CachedOwnerCharacter)))
	{
		return false;
	}

	if (SkillComponent->IsUsingSkill() || CachedOwnerCharacter->IsJumping())
	{
		return false;
	}

	return true;
}

void UNetherCrownActionControlComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheInitData();
}

void UNetherCrownActionControlComponent::CacheInitData()
{
	CachedOwnerCharacter = Cast<ANetherCrownCharacter>(GetOwner());

	if (!ensureAlways(IsValid(CachedOwnerCharacter)))
	{
		return;
	}

	BasicAttackComponentWeak = MakeWeakObjectPtr(CachedOwnerCharacter->GetBasicAttackComponent());
	SkillComponentWeak = MakeWeakObjectPtr(CachedOwnerCharacter->GetSkillComponent());
	EquipComponentWeak = MakeWeakObjectPtr(CachedOwnerCharacter->GetEquipComponent());
}
