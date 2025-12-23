// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEquipComponent.h"

#include "Net/UnrealNetwork.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Character/AnimInstance/NetherCrownCharacterAnimInstance.h"
#include "NetherCrown/Settings/NetherCrownCharacterDefaultSettings.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"
#include "NetherCrown/Weapon/NetherCrownWeapon.h"

UNetherCrownEquipComponent::UNetherCrownEquipComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void UNetherCrownEquipComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UNetherCrownEquipComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UNetherCrownEquipComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquippedWeapon);
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

void UNetherCrownEquipComponent::NotifyEquipEndOrStart(const bool bEquipEnd) const
{
	if (GetOwner()->HasAuthority())
	{
		OnEquipEndOrStart.Broadcast(bEquipEnd);
	}
}

const UNetherCrownWeaponData* UNetherCrownEquipComponent::GetEquippedWeaponData() const
{
	if (!IsValid(EquippedWeapon))
	{
		UE_LOG(LogTemp, Warning, TEXT("Equipped weapon is invalid. Returning nullptr. %hs"), __FUNCTION__);
		return nullptr;
	}

	return EquippedWeapon->GetWeaponData();
}

const FNetherCrownWeaponTagData& UNetherCrownEquipComponent::GetEquippedWeaponTagData() const
{
	return EquippedWeapon->GetWeaponTagData();
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

	if (OwnerCharacter->IsLocallyControlled())
	{
		FNetherCrownUtilManager::PlaySound2DByGameplayTag(GetWorld(), EquipComponentTagData.EquipSoundTag);
	}
}

void UNetherCrownEquipComponent::AttachWeaponToCharacterMesh(ANetherCrownWeapon* TargetWeapon, const FName& WeaponSocketName) const
{
	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	USkeletalMeshComponent* OwnerCharacterMesh{ OwnerCharacter ? OwnerCharacter->GetMesh() : nullptr };
	if (!ensureAlways(IsValid(OwnerCharacterMesh)))
	{
		return;
	}

	TargetWeapon->SetOwner(OwnerCharacter);
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

	if (IsValid(EquippedWeapon))
	{
		StowCurrentWeapon();
	}

	EquippedWeapon = EquipableWeapon;
	EquippedWeapon->DisableEquipSphereCollision();

	if (EquipableWeaponWeak.IsValid())
	{
		EquipableWeaponWeak->Reset();
	}

	const UNetherCrownCharacterDefaultSettings* CharacterDefaultSettings{ GetDefault<UNetherCrownCharacterDefaultSettings>() };
	check(CharacterDefaultSettings);

	const FName& EquipWeaponSocketName{ CharacterDefaultSettings->EquipWeaponSocketName };
	AttachWeaponToCharacterMesh(EquippedWeapon, EquipWeaponSocketName);
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
	AttachWeaponToCharacterMesh(EquippedWeapon, StowSocketName);

	StowWeaponContainer.Add(TPair<EStowWeaponPosition, ANetherCrownWeapon*>{ ChangeTargetWeaponPosition, EquippedWeapon });

	EquippedWeapon = ChangeTargetWeaponPair.Value;

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

	AttachWeaponToCharacterMesh(EquippedWeapon, StowWeaponSocketName);

	StowWeaponContainer.Add(TPair<EStowWeaponPosition, ANetherCrownWeapon*>{ StowWeaponPosition, EquippedWeapon });
}
