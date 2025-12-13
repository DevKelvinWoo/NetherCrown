// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEquipComponent.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Character/AnimInstance/NetherCrownCharacterAnimInstance.h"
#include "NetherCrown/Settings/NetherCrownCharacterDefaultSettings.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"
#include "NetherCrown/Weapon/NetherCrownWeapon.h"

UNetherCrownEquipComponent::UNetherCrownEquipComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UNetherCrownEquipComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UNetherCrownEquipComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UNetherCrownEquipComponent::SetEquipableWeapon(ANetherCrownWeapon* InEquipableWeapon)
{
	EquipableWeaponWeak = MakeWeakObjectPtr(InEquipableWeapon);
}

void UNetherCrownEquipComponent::EquipOrStowWeapon()
{
	Server_EquipOrStowWeapon();
}

void UNetherCrownEquipComponent::ChangeWeapon()
{
	Server_ChangeWeapon();
}

void UNetherCrownEquipComponent::Server_ChangeWeapon_Implementation()
{
	ChangeWeaponInternal();
}

void UNetherCrownEquipComponent::Server_EquipOrStowWeapon_Implementation()
{
	if (bCanEquip)
	{
		EquipOrStowWeaponInternal();
		Multicast_PlayEquipAnimationAndSound();

		OnEquipWeapon.Broadcast(true);
	}
}

void UNetherCrownEquipComponent::Multicast_PlayEquipAnimationAndSound_Implementation()
{
	const ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	const USkeletalMeshComponent* OwnerCharacterMesh{ OwnerCharacter ? OwnerCharacter->GetMesh() : nullptr };
	UAnimInstance* AnimInstance{ OwnerCharacterMesh ? OwnerCharacterMesh->GetAnimInstance() : nullptr};
	UNetherCrownCharacterAnimInstance* NetherCrownCharacterAnimInstance{ Cast<UNetherCrownCharacterAnimInstance>(AnimInstance) };
	if (!ensureAlways(IsValid(NetherCrownCharacterAnimInstance)))
	{
		return;
	}

	UAnimMontage* EquipAnimMontage{ EquipAnimMontageSoft.LoadSynchronous() };
	if (!ensureAlways(IsValid(EquipAnimMontage)))
	{
		return;
	}

	NetherCrownCharacterAnimInstance->Montage_Play(EquipAnimMontage);

	FNetherCrownUtilManager::PlaySound2DByGameplayTag(GetWorld(), NetherCrownTags::Sound_Character_EquipWeapon);
}

void UNetherCrownEquipComponent::AttachWeaponToCharacterMesh(ANetherCrownWeapon* TargetWeapon, const FName& WeaponSocketName) const
{
	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	USkeletalMeshComponent* OwnerCharacterMesh{ OwnerCharacter ? OwnerCharacter->GetMesh() : nullptr };
	if (!ensureAlways(IsValid(OwnerCharacterMesh)))
	{
		return;
	}

	TargetWeapon->AttachToComponent(OwnerCharacterMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);
}

void UNetherCrownEquipComponent::EquipOrStowWeaponInternal()
{
	ANetherCrownWeapon* EquipableWeapon{ EquipableWeaponWeak.Get() };
	if (!IsValid(EquipableWeapon))
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipableWeapon is invalid in %hs"), __FUNCTION__);
		return;
	}

	if (IsValid(EquipedWeapon))
	{
		StowCurrentWeapon();
	}

	EquipedWeapon = EquipableWeapon;
	EquipedWeapon->DisableEquipSphereCollision();

	if (EquipableWeaponWeak.IsValid())
	{
		EquipableWeaponWeak->Reset();
	}

	const UNetherCrownCharacterDefaultSettings* CharacterDefaultSettings{ GetDefault<UNetherCrownCharacterDefaultSettings>() };
	check(CharacterDefaultSettings);

	const FName& EquipWeaponSocketName{ CharacterDefaultSettings->EquipWeaponSocketName };
	AttachWeaponToCharacterMesh(EquipedWeapon, EquipWeaponSocketName);
}

void UNetherCrownEquipComponent::ChangeWeaponInternal()
{
	if (StowWeaponContainer.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("StowWeaponContainer is Empty %hs"), __FUNCTION__);
		return;
	}

	const UNetherCrownCharacterDefaultSettings* CharacterDefaultSettings{ GetDefault<UNetherCrownCharacterDefaultSettings>() };
	check(CharacterDefaultSettings);

	TPair<EStowWeaponPosition, ANetherCrownWeapon*> ChangeTargetWeaponPair{ StowWeaponContainer[0] };
	StowWeaponContainer.RemoveAt(0);

	const FName& WeaponSocketName{ CharacterDefaultSettings->EquipWeaponSocketName };
	AttachWeaponToCharacterMesh(ChangeTargetWeaponPair.Value, WeaponSocketName);

	const EStowWeaponPosition ChangeTargetWeaponPosition{ ChangeTargetWeaponPair.Key };
	FName StowSocketName{};
	ChangeTargetWeaponPosition == EStowWeaponPosition::Left ? StowSocketName = CharacterDefaultSettings->StowWeaponSocketLName : StowSocketName = CharacterDefaultSettings->WeaponHandleSocketRName;
	AttachWeaponToCharacterMesh(EquipedWeapon, StowSocketName);

	StowWeaponContainer.Add(TPair<EStowWeaponPosition, ANetherCrownWeapon*>{ ChangeTargetWeaponPosition, EquipedWeapon });

	EquipedWeapon = ChangeTargetWeaponPair.Value;

	Multicast_PlayEquipAnimationAndSound();
}

void UNetherCrownEquipComponent::StowCurrentWeapon()
{
	const UNetherCrownCharacterDefaultSettings* CharacterDefaultSettings{ GetDefault<UNetherCrownCharacterDefaultSettings>() };
	check(CharacterDefaultSettings);

	FName StowWeaponSocketName{};
	EStowWeaponPosition StowWeaponPosition{};
	if (StowWeaponContainer.IsEmpty())
	{
		StowWeaponPosition = EStowWeaponPosition::Left;
		StowWeaponSocketName = CharacterDefaultSettings->StowWeaponSocketLName;
	}
	else
	{
		StowWeaponPosition = EStowWeaponPosition::Right;
		StowWeaponSocketName = CharacterDefaultSettings->WeaponHandleSocketRName;
	}

	AttachWeaponToCharacterMesh(EquipedWeapon, StowWeaponSocketName);

	StowWeaponContainer.Add(TPair<EStowWeaponPosition, ANetherCrownWeapon*>{ StowWeaponPosition, EquipedWeapon });
}
