// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownBasicAttackComponent.h"
#include "Animation/AnimMontage.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Character/AnimInstance/NetherCrownCharacterAnimInstance.h"

void UNetherCrownBasicAttackComponent::RequestBasicAttack()
{
	Server_RequestBasicAttack();
}

void UNetherCrownBasicAttackComponent::Server_RequestBasicAttack_Implementation()
{
	if (bCanQueueNextCombo)
	{
		bHasQueuedNextCombo = true;
	}
	else if (bCanInputFirstAttack)
	{
		StartAttackBasic();
	}
}

void UNetherCrownBasicAttackComponent::StartAttackBasic()
{
	bCanInputFirstAttack = false;

	if (ComboMontageSectionMap.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("ComboMontageSectionMap is Empty in %hs"), __FUNCTION__);
		return;
	}

	const FName* FirstComboMontageSectionName{ ComboMontageSectionMap.Find(1) };
	Multicast_PlayAndJumpToComboMontageSection(*FirstComboMontageSectionName);
}

void UNetherCrownBasicAttackComponent::Multicast_PlayAndJumpToComboMontageSection_Implementation(const FName& SectionName)
{
	PlayAndJumpToComboMontageSection(&SectionName);
}

void UNetherCrownBasicAttackComponent::PlayAndJumpToComboMontageSection(const FName* SectionName) const
{
	if (!SectionName)
	{
		UE_LOG(LogTemp, Warning, TEXT("Montage section not found in %hs"), __FUNCTION__);
		return;
	}

	UAnimMontage* BasicAttackAnimMontage{ BasicAttackAnimMontageSoft.LoadSynchronous() };
	if (!ensureAlways(IsValid(BasicAttackAnimMontage)))
	{
		return;
	}

	const ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	const USkeletalMeshComponent* OwnerCharacterMesh{ OwnerCharacter ? OwnerCharacter->GetMesh() : nullptr };
	UAnimInstance* AnimInstance{ OwnerCharacterMesh ? OwnerCharacterMesh->GetAnimInstance() : nullptr};
	UNetherCrownCharacterAnimInstance* NetherCrownCharacterAnimInstance{ Cast<UNetherCrownCharacterAnimInstance>(AnimInstance) };
	if (!ensureAlways(IsValid(NetherCrownCharacterAnimInstance)))
	{
		return;
	}

	NetherCrownCharacterAnimInstance->Montage_Play(BasicAttackAnimMontage);
	NetherCrownCharacterAnimInstance->Montage_JumpToSection(*SectionName);
	//@NOTE : AnimMontage의 BlendOutTriggerTime을 0으로 Setting하여 Idle로 천천히 넘어가도록 제어하여 어색함을 없앰
}

void UNetherCrownBasicAttackComponent::CalculateNextComboCount()
{
	const int32 MaxComboCount{ ComboMontageSectionMap.Num() };
	CurrentComboCount = CurrentComboCount + 1 > MaxComboCount ? 1 : ++CurrentComboCount;

	UE_LOG(LogTemp, Warning, TEXT("ComboCount : %d"), CurrentComboCount);
}

void UNetherCrownBasicAttackComponent::EnableComboWindow()
{
	//@NOTE : Do not use AnimNotifyState (Server<->Client duration issue)
	AActor* Owner{ GetOwner() };
	check(Owner);

	if (Owner->HasAuthority())
	{
		bCanQueueNextCombo = true;
	}
}

void UNetherCrownBasicAttackComponent::DisableComboAndPlayQueuedComboWindow()
{
	//@NOTE : Do not use AnimNotifyState (Server<->Client duration issue)
	AActor* Owner{ GetOwner() };
	check(Owner);

	if (Owner->HasAuthority())
	{
		bCanQueueNextCombo = false;

		if (!bHasQueuedNextCombo)
		{
			CurrentComboCount = 1;
			bCanInputFirstAttack = true;

			return;
		}

		CalculateNextComboCount();

		bHasQueuedNextCombo = false;

		if (ComboMontageSectionMap.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("ComboMontageSectionMap is Empty in %hs"), __FUNCTION__);
			return;
		}

		const FName* FirstComboMontageSectionName{ ComboMontageSectionMap.Find(CurrentComboCount) };
		Multicast_PlayAndJumpToComboMontageSection(*FirstComboMontageSectionName);
	}
}
