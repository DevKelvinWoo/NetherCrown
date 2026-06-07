// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEquipComponent.h"

#include "NetherCrown/NetherCrown.h"
#include "Animation/AnimMontage.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Character/NetherCrownPlayerController.h"
#include "NetherCrown/Character/AnimInstance/NetherCrownCharacterAnimInstance.h"
#include "NetherCrown/PlayerState/NetherCrownPlayerState.h"
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
	DOREPLIFETIME(ThisClass, EquipState);
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

FNetherCrownWeaponPersistentData UNetherCrownEquipComponent::MakeWeaponPersistentData() const
{
	FNetherCrownWeaponPersistentData CurrentWeaponPersistentData{};

	const UNetherCrownCharacterDefaultSettings* CharacterDefaultSettings{ GetDefault<UNetherCrownCharacterDefaultSettings>() };
	check(CharacterDefaultSettings);

	ANetherCrownWeapon* AttachedEquippedWeapon{ FindAttachedWeaponBySocket(CharacterDefaultSettings->GetEquipWeaponSocketName()) };
	if (IsValid(AttachedEquippedWeapon))
	{
		CurrentWeaponPersistentData.EquippedWeaponClass = AttachedEquippedWeapon->GetClass();
	}

	ANetherCrownWeapon* AttachedLeftStowWeapon{ FindAttachedWeaponBySocket(CharacterDefaultSettings->GetStowWeaponSocketLName()) };
	if (IsValid(AttachedLeftStowWeapon))
	{
		CurrentWeaponPersistentData.LeftStowWeaponClass = AttachedLeftStowWeapon->GetClass();
	}

	ANetherCrownWeapon* AttachedRightStowWeapon{ FindAttachedWeaponBySocket(CharacterDefaultSettings->GetStowWeaponSocketRName()) };
	if (IsValid(AttachedRightStowWeapon))
	{
		CurrentWeaponPersistentData.RightStowWeaponClass = AttachedRightStowWeapon->GetClass();
	}

	if (IsValid(EquippedWeapon))
	{
		if (!CurrentWeaponPersistentData.EquippedWeaponClass)
		{
			CurrentWeaponPersistentData.EquippedWeaponClass = EquippedWeapon->GetClass();
		}
	}

	for (const TPair<ENetherCrownStowWeaponPosition, ANetherCrownWeapon*>& StowWeaponPair : StowWeaponContainer)
	{
		if (!IsValid(StowWeaponPair.Value))
		{
			continue;
		}

		if (StowWeaponPair.Key == ENetherCrownStowWeaponPosition::Left)
		{
			if (!CurrentWeaponPersistentData.LeftStowWeaponClass)
			{
				CurrentWeaponPersistentData.LeftStowWeaponClass = StowWeaponPair.Value->GetClass();
			}
		}
		else if (StowWeaponPair.Key == ENetherCrownStowWeaponPosition::Right)
		{
			if (!CurrentWeaponPersistentData.RightStowWeaponClass)
			{
				CurrentWeaponPersistentData.RightStowWeaponClass = StowWeaponPair.Value->GetClass();
			}
		}
	}

	return CurrentWeaponPersistentData;
}

bool UNetherCrownEquipComponent::RestoreWeaponFromPersistentData(const FNetherCrownWeaponPersistentData& InWeaponPersistentData)
{
	if (!IsValid(CachedCharacter))
	{
		CachedCharacter = Cast<ANetherCrownCharacter>(GetOwner());
	}

	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->HasAuthority())
	{
		return false;
	}

	WeaponPersistentData = InWeaponPersistentData;

	const UNetherCrownCharacterDefaultSettings* CharacterDefaultSettings{ GetDefault<UNetherCrownCharacterDefaultSettings>() };
	check(CharacterDefaultSettings);

	ClearRestoredWeapons();

	if (WeaponPersistentData.EquippedWeaponClass)
	{
		EquippedWeapon = SpawnPersistentWeapon(WeaponPersistentData.EquippedWeaponClass, CharacterDefaultSettings->GetEquipWeaponSocketName());
		if (!ensureAlways(IsValid(EquippedWeapon)))
		{
			return false;
		}
	}

	if (WeaponPersistentData.LeftStowWeaponClass)
	{
		ANetherCrownWeapon* LeftStowWeapon{ SpawnPersistentWeapon(WeaponPersistentData.LeftStowWeaponClass, CharacterDefaultSettings->GetStowWeaponSocketLName()) };
		if (!ensureAlways(IsValid(LeftStowWeapon)))
		{
			return false;
		}

		StowWeaponContainer.Add(TPair<ENetherCrownStowWeaponPosition, ANetherCrownWeapon*>{ ENetherCrownStowWeaponPosition::Left, LeftStowWeapon });
	}

	if (WeaponPersistentData.RightStowWeaponClass)
	{
		ANetherCrownWeapon* RightStowWeapon{ SpawnPersistentWeapon(WeaponPersistentData.RightStowWeaponClass, CharacterDefaultSettings->GetStowWeaponSocketRName()) };
		if (!ensureAlways(IsValid(RightStowWeapon)))
		{
			return false;
		}

		StowWeaponContainer.Add(TPair<ENetherCrownStowWeaponPosition, ANetherCrownWeapon*>{ ENetherCrownStowWeaponPosition::Right, RightStowWeapon });
	}

	EquipState = IsValid(EquippedWeapon) ? ENetherCrownEquipState::Equipped : ENetherCrownEquipState::Unequipped;
	OnEquipWeapon.Broadcast(IsValid(EquippedWeapon));
	SetWeaponPersistentData();

	return true;
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

	if (EquipData.EquipStartTimeOffset >= 0.f)
	{
		TimerManager.SetTimer(EquipStartTimerHandle, this, &ThisClass::HandleEquipStart, EquipData.EquipStartTimeOffset, false);
	}

	if (EquipData.EquipEndTimeOffset >= 0.f)
	{
		TimerManager.SetTimer(EquipEndTimerHandle, this, &ThisClass::HandleEquipEnd, EquipData.EquipEndTimeOffset, false);
	}
}

void UNetherCrownEquipComponent::HandleEquipStart()
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->HasAuthority())
	{
		return;
	}

	EquipState = ENetherCrownEquipState::Equipping;
}

void UNetherCrownEquipComponent::HandleEquipEnd()
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->HasAuthority())
	{
		return;
	}

	EquipState = ENetherCrownEquipState::Equipped;
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

void UNetherCrownEquipComponent::OnRep_EquippedWeapon()
{
	OnEquipWeapon.Broadcast(IsValid(EquippedWeapon));
}

ANetherCrownWeapon* UNetherCrownEquipComponent::GetStowedWeapon(const ENetherCrownStowWeaponPosition StowWeaponPosition) const
{
	const UNetherCrownCharacterDefaultSettings* CharacterDefaultSettings{ GetDefault<UNetherCrownCharacterDefaultSettings>() };
	check(CharacterDefaultSettings);

	FName StowWeaponSocketName{};
	if (StowWeaponPosition == ENetherCrownStowWeaponPosition::Left)
	{
		StowWeaponSocketName = CharacterDefaultSettings->GetStowWeaponSocketLName();
	}
	else if (StowWeaponPosition == ENetherCrownStowWeaponPosition::Right)
	{
		StowWeaponSocketName = CharacterDefaultSettings->GetStowWeaponSocketRName();
	}

	ANetherCrownWeapon* AttachedStowedWeapon{ FindAttachedWeaponBySocket(StowWeaponSocketName) };
	if (IsValid(AttachedStowedWeapon))
	{
		return AttachedStowedWeapon;
	}

	for (const TPair<ENetherCrownStowWeaponPosition, ANetherCrownWeapon*>& StowWeaponPair : StowWeaponContainer)
	{
		if (StowWeaponPair.Key == StowWeaponPosition && IsValid(StowWeaponPair.Value))
		{
			return StowWeaponPair.Value;
		}
	}

	return nullptr;
}

TArray<TPair<ENetherCrownStowWeaponPosition, ANetherCrownWeapon*>> UNetherCrownEquipComponent::GetStowedWeapons() const
{
	TArray<TPair<ENetherCrownStowWeaponPosition, ANetherCrownWeapon*>> ResultStowedWeapons{};

	ANetherCrownWeapon* LeftStowedWeapon{ GetStowedWeapon(ENetherCrownStowWeaponPosition::Left) };
	if (IsValid(LeftStowedWeapon))
	{
		ResultStowedWeapons.Add(TPair<ENetherCrownStowWeaponPosition, ANetherCrownWeapon*>{ ENetherCrownStowWeaponPosition::Left, LeftStowedWeapon });
	}

	ANetherCrownWeapon* RightStowedWeapon{ GetStowedWeapon(ENetherCrownStowWeaponPosition::Right) };
	if (IsValid(RightStowedWeapon))
	{
		ResultStowedWeapons.Add(TPair<ENetherCrownStowWeaponPosition, ANetherCrownWeapon*>{ ENetherCrownStowWeaponPosition::Right, RightStowedWeapon });
	}

	return ResultStowedWeapons;
}

void UNetherCrownEquipComponent::Server_ChangeWeapon_Implementation()
{
	ChangeWeaponInternal();
}

bool UNetherCrownEquipComponent::Server_ChangeWeapon_Validate()
{
	if (!IsValid(CachedCharacter))
	{
		return false;
	}

	return true;
}

void UNetherCrownEquipComponent::Server_EquipOrStowWeapon_Implementation()
{
	if (EquipState == ENetherCrownEquipState::Equipping)
	{
		return;
	}

	ANetherCrownWeapon* EquipableWeapon{ EquipableWeaponWeak.Get() };
	if (IsValid(EquipableWeapon))
	{
		constexpr float MaxEquipDistance{ 400.f };
		if (FVector::DistSquared(CachedCharacter->GetActorLocation(), EquipableWeapon->GetActorLocation()) > FMath::Square(MaxEquipDistance))
		{
			return;
		}
	}

	if (bCanEquip)
	{
		EquipOrStowWeaponInternal();
		SetupEquipStateTimer();
		Multicast_PlayEquipAnimationAndSound();

		OnEquipWeapon.Broadcast(true);
	}
}

bool UNetherCrownEquipComponent::Server_EquipOrStowWeapon_Validate()
{
	if (!IsValid(CachedCharacter))
	{
		return false;
	}

	return true;
}

void UNetherCrownEquipComponent::Multicast_PlayEquipAnimationAndSound_Implementation()
{
	if (!ensureAlways(IsValid(CachedCharacter)) || CachedCharacter->GetNetMode() == NM_DedicatedServer)
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

	if (CachedCharacter->IsLocallyControlled())
	{
		FNetherCrownUtilManager::PlaySound2DByGameplayTag(GetWorld(), EquipData.EquipTagData.EquipSoundTag);
	}
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
	EquippedWeapon->SetActiveWeaponIdleAuraNiagaraComponent(false);

	if (EquipableWeaponWeak.IsValid())
	{
		EquipableWeaponWeak->Reset();
	}

	const UNetherCrownCharacterDefaultSettings* CharacterDefaultSettings{ GetDefault<UNetherCrownCharacterDefaultSettings>() };
	check(CharacterDefaultSettings);

	const FName& EquipWeaponSocketName{ CharacterDefaultSettings->GetEquipWeaponSocketName() };
	AttachWeaponToCharacterMesh(EquippedWeapon, EquipWeaponSocketName);

	WeaponPersistentData.EquippedWeaponClass = EquippedWeapon.GetClass();

	SetWeaponPersistentData();
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

	const TPair<ENetherCrownStowWeaponPosition, ANetherCrownWeapon*> ChangeTargetWeaponPair{ StowWeaponContainer[0] };
	StowWeaponContainer.RemoveAt(0);

	const FName& WeaponSocketName{ CharacterDefaultSettings->GetEquipWeaponSocketName() };
	AttachWeaponToCharacterMesh(ChangeTargetWeaponPair.Value, WeaponSocketName);
	WeaponPersistentData.EquippedWeaponClass = ChangeTargetWeaponPair.Value->GetClass();

	const ENetherCrownStowWeaponPosition ChangeTargetWeaponPosition{ ChangeTargetWeaponPair.Key };
	FName StowSocketName{};
	if (ChangeTargetWeaponPosition == ENetherCrownStowWeaponPosition::Left)
	{
		StowSocketName = CharacterDefaultSettings->GetStowWeaponSocketLName();
		WeaponPersistentData.LeftStowWeaponClass = EquippedWeapon.GetClass();
	}
	else if (ChangeTargetWeaponPosition == ENetherCrownStowWeaponPosition::Right)
	{
		StowSocketName = CharacterDefaultSettings->GetStowWeaponSocketRName();
		WeaponPersistentData.RightStowWeaponClass = EquippedWeapon.GetClass();
	}

	AttachWeaponToCharacterMesh(EquippedWeapon, StowSocketName);

	StowWeaponContainer.Add(TPair<ENetherCrownStowWeaponPosition, ANetherCrownWeapon*>{ ChangeTargetWeaponPosition, EquippedWeapon });

	EquippedWeapon = ChangeTargetWeaponPair.Value;

	SetupEquipStateTimer();
	Multicast_PlayEquipAnimationAndSound();
	SetWeaponPersistentData();
}

void UNetherCrownEquipComponent::StowCurrentWeapon()
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->HasAuthority())
	{
		return;
	}


	const UNetherCrownCharacterDefaultSettings* CharacterDefaultSettings{ GetDefault<UNetherCrownCharacterDefaultSettings>() };
	check(CharacterDefaultSettings);

	FName StowWeaponSocketName{};
	ENetherCrownStowWeaponPosition StowWeaponPosition{};
	if (StowWeaponContainer.IsEmpty())
	{
		StowWeaponPosition = ENetherCrownStowWeaponPosition::Left;
		StowWeaponSocketName = CharacterDefaultSettings->GetStowWeaponSocketLName();

		WeaponPersistentData.LeftStowWeaponClass = EquippedWeapon.GetClass();
	}
	else
	{
		StowWeaponPosition = ENetherCrownStowWeaponPosition::Right;
		StowWeaponSocketName = CharacterDefaultSettings->GetStowWeaponSocketRName();

		WeaponPersistentData.RightStowWeaponClass = EquippedWeapon.GetClass();
	}

	AttachWeaponToCharacterMesh(EquippedWeapon, StowWeaponSocketName);

	StowWeaponContainer.Add(TPair<ENetherCrownStowWeaponPosition, ANetherCrownWeapon*>{ StowWeaponPosition, EquippedWeapon });
}

ANetherCrownWeapon* UNetherCrownEquipComponent::SpawnPersistentWeapon(const TSubclassOf<ANetherCrownWeapon> WeaponClass, const FName& SocketName)
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->HasAuthority() || !WeaponClass)
	{
		return nullptr;
	}

	UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParameters{};
	SpawnParameters.Owner = CachedCharacter;
	SpawnParameters.Instigator = CachedCharacter;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ANetherCrownWeapon* SpawnedWeapon{ World->SpawnActor<ANetherCrownWeapon>(WeaponClass, CachedCharacter->GetActorTransform(), SpawnParameters) };
	if (!ensureAlways(IsValid(SpawnedWeapon)))
	{
		return nullptr;
	}

	SpawnedWeapon->DisableEquipSphereCollision();
	AttachWeaponToCharacterMesh(SpawnedWeapon, SocketName);

	return SpawnedWeapon;
}

ANetherCrownWeapon* UNetherCrownEquipComponent::FindAttachedWeaponBySocket(const FName& SocketName) const
{
	if (SocketName.IsNone())
	{
		return nullptr;
	}

	const ANetherCrownCharacter* OwnerCharacter{ CachedCharacter };
	if (!IsValid(OwnerCharacter))
	{
		OwnerCharacter = Cast<ANetherCrownCharacter>(GetOwner());
	}

	if (!IsValid(OwnerCharacter))
	{
		return nullptr;
	}

	TArray<AActor*> AttachedActors{};
	OwnerCharacter->GetAttachedActors(AttachedActors, false, true);

	for (AActor* AttachedActor : AttachedActors)
	{
		ANetherCrownWeapon* AttachedWeapon{ Cast<ANetherCrownWeapon>(AttachedActor) };
		if (IsValid(AttachedWeapon) && AttachedWeapon->GetAttachParentSocketName() == SocketName)
		{
			return AttachedWeapon;
		}
	}

	return nullptr;
}

void UNetherCrownEquipComponent::ClearRestoredWeapons()
{
	if (IsValid(EquippedWeapon))
	{
		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;
	}

	for (const TPair<ENetherCrownStowWeaponPosition, ANetherCrownWeapon*>& StowWeaponPair : StowWeaponContainer)
	{
		if (IsValid(StowWeaponPair.Value))
		{
			StowWeaponPair.Value->Destroy();
		}
	}

	StowWeaponContainer.Empty();
	EquipableWeaponWeak.Reset();
}

void UNetherCrownEquipComponent::CacheInitData()
{
	CachedCharacter = Cast<ANetherCrownCharacter>(GetOwner());

	LoadEquipData();

	if (!ensureAlways(IsValid(CachedCharacter)) || CachedCharacter->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (EquipData.EquipAnimMontageSoft.IsNull())
	{
		return;
	}

	CachedEquipMontage = EquipData.EquipAnimMontageSoft.LoadSynchronous();
}

void UNetherCrownEquipComponent::LoadEquipData()
{
	if (EquipDataAssetSoft.IsNull())
	{
		return;
	}

	const UNetherCrownEquipDataAsset* EquipDataAsset{ EquipDataAssetSoft.LoadSynchronous() };
	if (!ensureAlways(IsValid(EquipDataAsset)))
	{
		return;
	}

	EquipData = EquipDataAsset->GetEquipData();
}

void UNetherCrownEquipComponent::SetWeaponPersistentData()
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->HasAuthority())
	{
		return;
	}

	const ANetherCrownPlayerController* PlayerController{ Cast<ANetherCrownPlayerController>(CachedCharacter->GetController()) };
	if (!ensureAlways(IsValid(PlayerController)))
	{
		return;
	}

	ANetherCrownPlayerState* PlayerState{ PlayerController->GetPlayerState<ANetherCrownPlayerState>() };
	if (!ensureAlways(IsValid(PlayerState)))
	{
		return;
	}

	PlayerState->SetWeaponPersistentData(WeaponPersistentData);
}
