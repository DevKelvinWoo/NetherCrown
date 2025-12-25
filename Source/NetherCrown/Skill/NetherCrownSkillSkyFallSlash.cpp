// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownSkillSkyFallSlash.h"

#include "NetherCrown/Character/AnimInstance/NetherCrownCharacterAnimInstance.h"

void UNetherCrownSkillSkyFallSlash::ActiveSkill()
{
	Super::ActiveSkill();

	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	const USkeletalMeshComponent* SkeletalMeshComponent{ SkillOwnerCharacter->GetMesh() };
	UNetherCrownCharacterAnimInstance* NetherCrownCharacterAnimInstance{};
	NetherCrownCharacterAnimInstance = SkeletalMeshComponent ? Cast<UNetherCrownCharacterAnimInstance>(SkeletalMeshComponent->GetAnimInstance()) : nullptr;
	if (!ensureAlways(IsValid(NetherCrownCharacterAnimInstance)))
	{
		return;
	}

	UAnimMontage* SkillAnimMontage{ SkillAnimMontageSoft.LoadSynchronous() };
	if (!ensureAlways(IsValid(SkillAnimMontage)))
	{
		return;
	}

	//@TODO : Multicast로 재생해야 함, Section명 변경 필요
	NetherCrownCharacterAnimInstance->Montage_Play(SkillAnimMontage);
	NetherCrownCharacterAnimInstance->Montage_JumpToSection(TEXT("Default"));
}
