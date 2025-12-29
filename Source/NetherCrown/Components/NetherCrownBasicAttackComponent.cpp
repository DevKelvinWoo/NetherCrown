// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownBasicAttackComponent.h"

#include "NetherCrownEquipComponent.h"
#include "NetherCrownPlayerStatComponent.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Character/NetherCrownPlayerController.h"
#include "NetherCrown/Character/AnimInstance/NetherCrownCharacterAnimInstance.h"
#include "NetherCrown/Data/NetherCrownWeaponData.h"
#include "NetherCrown/PlayerState/NetherCrownPlayerState.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"
#include "NetherCrown/Weapon/NetherCrownWeapon.h"

void UNetherCrownBasicAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)))
	{
		return;
	}

	UNetherCrownEquipComponent* EquipComponent{ OwnerCharacter->GetEquipComponent() };
	if (!ensureAlways(IsValid(EquipComponent)))
	{
		return;
	}

	EquipComponent->GetOnEquipWeapon().AddUObject(this, &ThisClass::HandleOnEquipWeapon);
}

void UNetherCrownBasicAttackComponent::RequestBasicAttack()
{
	Server_RequestBasicAttack();
}

void UNetherCrownBasicAttackComponent::Server_RequestBasicAttack_Implementation()
{
	if (!bCanAttack)
	{
		return;
	}

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
	PlayAttackSoundAndJumpToComboMontageSection(&SectionName);
}

void UNetherCrownBasicAttackComponent::PlayAttackSoundAndJumpToComboMontageSection(const FName* SectionName) const
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

	OnStopOrStartBasicAttackAnim.Broadcast(false);

	PlayBasicAttackSounds();
}

void UNetherCrownBasicAttackComponent::SetEquippedWeaponTraceEnable(const bool bEnable) const
{
	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)))
	{
		return;
	}

	UNetherCrownEquipComponent* EquipComponent{ OwnerCharacter->GetEquipComponent() };
	if (!ensureAlways(IsValid(EquipComponent)))
	{
		return;
	}

	const ANetherCrownWeapon* EquippedWeapon{ EquipComponent->GetEquippedWeapon() };
	if (!ensureAlways(IsValid(EquippedWeapon)))
	{
		return;
	}

	EquippedWeapon->SetWeaponHitTraceEnable(bEnable);
	EquippedWeapon->InitWeaponTraceComponentSettings();
}

void UNetherCrownBasicAttackComponent::Server_ApplyDamageToHitEnemy_Implementation(AActor* HitEnemy)
{
	ApplyDamageInternal(HitEnemy);
}

void UNetherCrownBasicAttackComponent::Server_SpawnHitImpactEffect_Implementation(const FVector& HitLocation)
{
	Multicast_PlayHitImpactEffect(HitLocation);
}

void UNetherCrownBasicAttackComponent::Multicast_PlayHitImpactEffect_Implementation(const FVector& HitLocation)
{
	SpawnHitImpactEffect(HitLocation);
}

void UNetherCrownBasicAttackComponent::ApplyDamageInternal(AActor* HitEnemy) const
{
	UGameplayStatics::ApplyDamage(HitEnemy, CalculateBasicAttackDamage(), GetOwner()->GetInstigatorController(), GetOwner(), UDamageType::StaticClass());
}

void UNetherCrownBasicAttackComponent::HandleOnEquipWeapon(const bool bEquipWeapon)
{
	if (GetOwner()->HasAuthority())
	{
		bCanAttack = bEquipWeapon;
	}
}

int32 UNetherCrownBasicAttackComponent::CalculateBasicAttackDamage() const
{
	//@NOTE : Character의 Stat의 공격력, 현재 장착중인 Weapon의 공격력을 이용하여 데미지 계산을 한다
	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)))
	{
		return 0;
	}

	const ANetherCrownPlayerState* OwnerPlayerState{ Cast<ANetherCrownPlayerState>(OwnerCharacter->GetPlayerState()) };
	check(OwnerPlayerState);

	const UNetherCrownPlayerStatComponent* OwnerChracterStatComponent{ OwnerPlayerState->GetNetherCrownPlayerStatComponent() };
	if (!ensureAlways(IsValid(OwnerChracterStatComponent)))
	{
		return 0;
	}

	const FNetherCrownPlayerStatData& StatData{ OwnerChracterStatComponent->GetPlayerStatData() };
	const int32 AttackDamage{ StatData.AttackDamage };

	const UNetherCrownEquipComponent* EquipComponent{ OwnerCharacter->GetEquipComponent() };
	if (!ensureAlways(IsValid(EquipComponent)))
	{
		return 0;
	}

	const UNetherCrownWeaponData* EquippedWeaponData{ EquipComponent->GetEquippedWeaponData() };
	const int32 WeaponAttackDamage{ EquippedWeaponData ? EquippedWeaponData->WeaponAttackDamage : 0 };

	return AttackDamage + WeaponAttackDamage;
}

void UNetherCrownBasicAttackComponent::PlayHitImpactCameraShake() const
{
	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)))
	{
		return;
	}

	ANetherCrownPlayerController* OwnerPlayerController{ Cast<ANetherCrownPlayerController>(OwnerCharacter->GetController()) };
	check(OwnerPlayerController);

	APlayerCameraManager* CameraManager{ OwnerPlayerController->PlayerCameraManager };
	check(CameraManager);

	CameraManager->StartCameraShake(ApplyDamageCameraShakeClass, 1.0f);
}

void UNetherCrownBasicAttackComponent::PlayBasicAttackSounds() const
{
	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)))
	{
		return;
	}

	FNetherCrownUtilManager::PlaySound2DByGameplayTag(GetOwner(), BasicAttackComponentTagData.BasicAttackGruntSoundTag);

	UNetherCrownEquipComponent* EquipComponent{ OwnerCharacter->GetEquipComponent() };
	if (!ensureAlways(EquipComponent))
	{
		return;
	}

	const FGameplayTag& SwingWeaponSoundTag{ EquipComponent->GetEquippedWeaponTagData().WeaponSwingSound };
	FNetherCrownUtilManager::PlaySound2DByGameplayTag(GetOwner(), SwingWeaponSoundTag);
}

void UNetherCrownBasicAttackComponent::SpawnHitImpactEffect(const FVector& HitLocation) const
{
	FTransform SpawnTransform{};
	SpawnTransform.SetLocation(HitLocation);
	SpawnTransform.SetRotation(FRotator::ZeroRotator.Quaternion());
	SpawnTransform.SetScale3D(FVector(1.0f));

	FNetherCrownUtilManager::SpawnNiagaraSystemByGameplayTag(GetOwner(), BasicAttackComponentTagData.BasicAttackImpactEffectTag, SpawnTransform);
}

void UNetherCrownBasicAttackComponent::ApplyDamageToHitEnemy(AActor* HitEnemy, const FVector& HitLocation)
{
	AActor* OwnerActor = GetOwner();
	ANetherCrownCharacter* OwnerCharacter = Cast<ANetherCrownCharacter>(OwnerActor);
	if (!ensureAlways(IsValid(OwnerCharacter)))
	{
		return;
	}

	if (!OwnerCharacter->IsLocallyControlled()) //@NOTE : SimulatedProxy Role Character의 실행 방지
	{
		return;
	}

	if (!IsValid(HitEnemy))
	{
		return;
	}

	PlayHitImpactCameraShake();

	Server_SpawnHitImpactEffect(HitLocation);
	Server_ApplyDamageToHitEnemy(HitEnemy);
}

void UNetherCrownBasicAttackComponent::CalculateNextComboCount()
{
	const int32 MaxComboCount{ ComboMontageSectionMap.Num() };
	CurrentComboCount = CurrentComboCount + 1 > MaxComboCount ? 1 : ++CurrentComboCount;
}

void UNetherCrownBasicAttackComponent::HandleEnableComboWindow()
{
	//@NOTE : Do not use AnimNotifyState (Server<->Client duration issue)
	AActor* Owner{ GetOwner() };
	check(Owner);

	if (Owner->HasAuthority())
	{
		bCanQueueNextCombo = true;
	}
}

void UNetherCrownBasicAttackComponent::HandleDisableComboWindow()
{
	//@NOTE : Do not use AnimNotifyState (Server<->Client duration issue)
	AActor* Owner{ GetOwner() };
	check(Owner);

	if (Owner->HasAuthority())
	{
		bCanQueueNextCombo = false;

		if (!bHasQueuedNextCombo)
		{
			SetEquippedWeaponTraceEnable(false);

			CurrentComboCount = 1;
			bCanInputFirstAttack = true;

			OnStopOrStartBasicAttackAnim.Broadcast(true);

			return;
		}

		CalculateNextComboCount();

		bHasQueuedNextCombo = false;

		if (ComboMontageSectionMap.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("ComboMontageSectionMap is Empty in %hs"), __FUNCTION__);
			return;
		}

		const FName* CurrentComboMontageSectionName{ ComboMontageSectionMap.Find(CurrentComboCount) };
		Multicast_PlayAndJumpToComboMontageSection(*CurrentComboMontageSectionName);
	}

	SetEquippedWeaponTraceEnable(false);
}

void UNetherCrownBasicAttackComponent::HandleEnableHitTrace()
{
	SetEquippedWeaponTraceEnable(true);
}
