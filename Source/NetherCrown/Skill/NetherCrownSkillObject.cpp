// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownSkillObject.h"

#include "Net/UnrealNetwork.h"
#include "NetherCrown/Character/AnimInstance/NetherCrownCharacterAnimInstance.h"
#include "TimerManager.h"

void UNetherCrownSkillObject::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, SkillKeyEnum);
	DOREPLIFETIME(ThisClass, SkillOwnerCharacterWeak);
	DOREPLIFETIME(ThisClass, SkillMontageBeginSlowPlayRate);
	DOREPLIFETIME(ThisClass, SkillMontageEndSlowPlayRate);
}

void UNetherCrownSkillObject::PlaySkillCosmetics()
{
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

	NetherCrownCharacterAnimInstance->Montage_Play(SkillAnimMontage);
}

void UNetherCrownSkillObject::SetSkillMontageSlowPlayRate(float InPlayRate) const
{
	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	USkeletalMeshComponent* SkeletalMeshComponent{ SkillOwnerCharacter ? SkillOwnerCharacter->GetMesh() : nullptr };
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

	NetherCrownCharacterAnimInstance->Montage_SetPlayRate(SkillAnimMontage, InPlayRate);
}
