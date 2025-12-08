// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEquipComponent.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Weapon/NetherCrownWeapon.h"

UNetherCrownEquipComponent::UNetherCrownEquipComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UNetherCrownEquipComponent::SetEquipableWeapon(ANetherCrownWeapon* InEquipableWeapon)
{
	EquipableWeaponWeak = MakeWeakObjectPtr(InEquipableWeapon);
}

void UNetherCrownEquipComponent::EquipWeapon()
{
	Server_EquipWeapon();
}

void UNetherCrownEquipComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UNetherCrownEquipComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UNetherCrownEquipComponent::Server_EquipWeapon_Implementation()
{
	ANetherCrownWeapon* EquipableWeapon{ EquipableWeaponWeak.Get() };
	if (!IsValid(EquipableWeapon))
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipableWeapon is invalid in %hs"), __FUNCTION__);
		return;
	}

	EquipedWeapon = EquipableWeapon;
	EquipableWeaponWeak->Reset();

	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	USkeletalMeshComponent* OwnerCharacterMesh{ OwnerCharacter ? OwnerCharacter->GetMesh() : nullptr };
	if (!ensureAlways(IsValid(OwnerCharacterMesh)))
	{
		return;
	}

	EquipedWeapon->AttachToComponent(OwnerCharacterMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);
}
