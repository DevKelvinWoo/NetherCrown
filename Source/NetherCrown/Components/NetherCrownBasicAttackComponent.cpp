// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownBasicAttackComponent.h"
#include "Animation/AnimMontage.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Character/AnimInstance/NetherCrownCharacterAnimInstance.h"

void UNetherCrownBasicAttackComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bCanInputFirstAttack);
}

void UNetherCrownBasicAttackComponent::RequestBasicAttack()
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
	Server_SetCanInputFirstAttack(false);
	bCanInputFirstAttack = false;

	Server_PlayAndJumpToComboMontageSection();
}

void UNetherCrownBasicAttackComponent::Server_PlayAndJumpToComboMontageSection_Implementation()
{
	if (ComboMontageSectionMap.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("ComboMontageSectionMap is Empty in %hs"), __FUNCTION__);
		return;
	}

	const FName* FirstComboMontageSectionName{ ComboMontageSectionMap.Find(CurrentComboCount) };
	Multicast_PlayAndJumpToComboMontageSection(*FirstComboMontageSectionName);
}

void UNetherCrownBasicAttackComponent::Server_PlayNextComboSection_Implementation()
{
	if (ComboMontageSectionMap.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("ComboMontageSectionMap is Empty in %hs"), __FUNCTION__);
		return;
	}

	CalculateNextComboCount();

	const FName* NextComboMontageSectionName{ ComboMontageSectionMap.Find(CurrentComboCount) };
	Multicast_PlayAndJumpToComboMontageSection(*NextComboMontageSectionName);
}

void UNetherCrownBasicAttackComponent::Multicast_PlayAndJumpToComboMontageSection_Implementation(const FName& SectionName)
{
	PlayAndJumpToComboMontageSection(&SectionName);
}

void UNetherCrownBasicAttackComponent::Server_SetCanInputFirstAttack_Implementation(const bool InbCanInputFirstAttack)
{
	bCanInputFirstAttack = InbCanInputFirstAttack;
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
}

void UNetherCrownBasicAttackComponent::EnableComboWindow()
{
	bCanQueueNextCombo = true;
}

void UNetherCrownBasicAttackComponent::DisableComboAndPlayQueuedComboWindow()
{
	bCanQueueNextCombo = false;

	if (!bHasQueuedNextCombo)
	{
		bCanInputFirstAttack = true;

		return;
	}

	bHasQueuedNextCombo = false;

	Server_PlayNextComboSection();
}
