// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEquipComponent.h"

#include "NetherCrown/NetherCrown.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Character/AnimInstance/NetherCrownCharacterAnimInstance.h"
#include "NetherCrown/Settings/NetherCrownCharacterDefaultSettings.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"
#include "NetherCrown/Weapon/NetherCrownWeapon.h"

UNetherCrownEquipComponent::UNetherCrownEquipComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UNetherCrownEquipComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquippedWeapon);
}

void UNetherCrownEquipComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheInitData();
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
	if (ensureAlways(IsValid(CachedCharacter)) && CachedCharacter->HasAuthority())
	{
		OnEquipEndOrStart.Broadcast(bEquipEnd);
	}
}

void UNetherCrownEquipComponent::SetupEquipStateTimer()
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->HasAuthority())
	{
		return;
	}

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	FTimerManager& TimerManager{ World->GetTimerManager() };
	TimerManager.ClearTimer(EquipStartTimerHandle);
	TimerManager.ClearTimer(EquipEndTimerHandle);

	if (EquipStartTimeOffset >= 0.f)
	{
		TimerManager.SetTimer(EquipStartTimerHandle, this, &ThisClass::HandleEquipStart, EquipStartTimeOffset, false);
	}

	if (EquipEndTimeOffset >= 0.f)
	{
		TimerManager.SetTimer(EquipEndTimerHandle, this, &ThisClass::HandleEquipEnd, EquipEndTimeOffset, false);
	}
}

void UNetherCrownEquipComponent::HandleEquipStart() const
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->HasAuthority())
	{
		return;
	}

	NotifyEquipEndOrStart(false);
}

void UNetherCrownEquipComponent::HandleEquipEnd() const
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->HasAuthority())
	{
		return;
	}

	NotifyEquipEndOrStart(true);
}

const UNetherCrownWeaponData* UNetherCrownEquipComponent::GetEquippedWeaponData() const
{
	if (!IsValid(EquippedWeapon))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("Equipped weapon is invalid. Returning nullptr. %hs"), __FUNCTION__);
		return nullptr;
	}

	return EquippedWeapon->GetWeaponData();
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
		SetupEquipStateTimer();
		Multicast_PlayEquipAnimationAndSound();

		OnEquipWeapon.Broadcast(true);
	}
}

void UNetherCrownEquipComponent::Multicast_PlayEquipAnimationAndSound_Implementation()
{
	if (!ensureAlways(IsValid(CachedCharacter)) || CachedCharacter->HasAuthority())
	{
		return;
	}

	const USkeletalMeshComponent* OwnerCharacterMesh{ CachedCharacter->GetMesh() };
	if (!ensureAlways(IsValid(OwnerCharacterMesh)))
	{
		return;
	}

	UAnimInstance* AnimInstance{ OwnerCharacterMesh->GetAnimInstance() };
	UNetherCrownCharacterAnimInstance* NetherCrownCharacterAnimInstance{ Cast<UNetherCrownCharacterAnimInstance>(AnimInstance) };
	if (!ensureAlways(IsValid(NetherCrownCharacterAnimInstance)))
	{
		return;
	}

	if (!ensureAlways(IsValid(CachedEquipMontage)))
	{
		return;
	}

	NetherCrownCharacterAnimInstance->Montage_Play(CachedEquipMontage);

	FNetherCrownUtilManager::PlaySound2DByGameplayTag(GetWorld(), EquipComponentTagData.EquipSoundTag);
}

void UNetherCrownEquipComponent::AttachWeaponToCharacterMesh(ANetherCrownWeapon* TargetWeapon, const FName& WeaponSocketName) const
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->HasAuthority())
	{
		return;
	}

	USkeletalMeshComponent* OwnerCharacterMesh{ CachedCharacter->GetMesh() };
	if (!ensureAlways(IsValid(OwnerCharacterMesh)))
	{
		return;
	}

	TargetWeapon->SetOwner(CachedCharacter);
	TargetWeapon->AttachToComponent(OwnerCharacterMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);
}

void UNetherCrownEquipComponent::EquipOrStowWeaponInternal()
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->HasAuthority())
	{
		return;
	}

	ANetherCrownWeapon* EquipableWeapon{ EquipableWeaponWeak.Get() };
	if (!IsValid(EquipableWeapon))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("EquipableWeapon is invalid in %hs"), __FUNCTION__);
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

	const FName& EquipWeaponSocketName{ CharacterDefaultSettings->GetEquipWeaponSocketName() };
	AttachWeaponToCharacterMesh(EquippedWeapon, EquipWeaponSocketName);
}

void UNetherCrownEquipComponent::ChangeWeaponInternal()
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->HasAuthority())
	{
		return;
	}

	if (StowWeaponContainer.IsEmpty())
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("StowWeaponContainerMap is Empty %hs"), __FUNCTION__);
		return;
	}

	const UNetherCrownCharacterDefaultSettings* CharacterDefaultSettings{ GetDefault<UNetherCrownCharacterDefaultSettings>() };
	check(CharacterDefaultSettings);

	TPair<EStowWeaponPosition, ANetherCrownWeapon*> ChangeTargetWeaponPair{ StowWeaponContainer[0] };
	StowWeaponContainer.RemoveAt(0);

	const FName& WeaponSocketName{ CharacterDefaultSettings->GetEquipWeaponSocketName() };
	AttachWeaponToCharacterMesh(ChangeTargetWeaponPair.Value, WeaponSocketName);

	const EStowWeaponPosition ChangeTargetWeaponPosition{ ChangeTargetWeaponPair.Key };
	FName StowSocketName{};
	ChangeTargetWeaponPosition == EStowWeaponPosition::Left ? StowSocketName = CharacterDefaultSettings->GetStowWeaponSocketLName() : StowSocketName = CharacterDefaultSettings->GetStowWeaponSocketRName();
	AttachWeaponToCharacterMesh(EquippedWeapon, StowSocketName);

	StowWeaponContainer.Add(TPair<EStowWeaponPosition, ANetherCrownWeapon*>{ ChangeTargetWeaponPosition, EquippedWeapon });

	EquippedWeapon = ChangeTargetWeaponPair.Value;

	SetupEquipStateTimer();
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
		StowWeaponSocketName = CharacterDefaultSettings->GetStowWeaponSocketLName();
	}
	else
	{
		StowWeaponPosition = EStowWeaponPosition::Right;
		StowWeaponSocketName = CharacterDefaultSettings->GetStowWeaponSocketRName();
	}

	AttachWeaponToCharacterMesh(EquippedWeapon, StowWeaponSocketName);

	StowWeaponContainer.Add(TPair<EStowWeaponPosition, ANetherCrownWeapon*>{ StowWeaponPosition, EquippedWeapon });
}

void UNetherCrownEquipComponent::CacheInitData()
{
	CachedCharacter = Cast<ANetherCrownCharacter>(GetOwner());

	if (!EquipAnimMontageSoft.IsNull() && !CachedCharacter->HasAuthority())
	{
		CachedEquipMontage = EquipAnimMontageSoft.LoadSynchronous();
	}
}
