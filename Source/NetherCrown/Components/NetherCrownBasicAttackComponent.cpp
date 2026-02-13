// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownBasicAttackComponent.h"

#include "NetherCrownEquipComponent.h"
#include "NetherCrownPlayerStatComponent.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Character/NetherCrownPlayerController.h"
#include "NetherCrown/Character/AnimInstance/NetherCrownCharacterAnimInstance.h"
#include "NetherCrown/Data/NetherCrownWeaponData.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/PlayerState/NetherCrownPlayerState.h"
#include "NetherCrown/Util/NetherCrownCollisionChannels.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"
#include "NetherCrown/Weapon/NetherCrownWeapon.h"

UNetherCrownBasicAttackComponent::UNetherCrownBasicAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UNetherCrownBasicAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheCharacter();

	if (!ensureAlways(IsValid(CachedCharacter)))
	{
		return;
	}

	UNetherCrownEquipComponent* EquipComponent{ CachedCharacter->GetEquipComponent() };
	if (!ensureAlways(IsValid(EquipComponent)))
	{
		return;
	}

	EquipComponent->GetOnEquipWeapon().AddUObject(this, &ThisClass::HandleOnEquipWeapon);

	CacheBasicAttackMontage();
}

void UNetherCrownBasicAttackComponent::CacheBasicAttackMontage()
{
	if (BasicAttackAnimMontageSoft.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("BasicAttackAnimMontage is null %hs"), __FUNCTION__);
		return;
	}

	CachedBasicAttackMontage = BasicAttackAnimMontageSoft.LoadSynchronous();
}

void UNetherCrownBasicAttackComponent::CacheCharacter()
{
	CachedCharacter = Cast<ANetherCrownCharacter>(GetOwner());
}

void UNetherCrownBasicAttackComponent::RequestBasicAttack()
{
	AutoTargetEnemy();

	Server_RequestBasicAttack();
}

void UNetherCrownBasicAttackComponent::Server_RequestBasicAttack_Implementation()
{
	if (BasicAttackState == ENetherCrownBasicAttackState::CannotAttack)
	{
		return;
	}

	Multicast_AutoTargetEnemy();

	if (BasicAttackState == ENetherCrownBasicAttackState::CanQueueNextCombo)
	{
		BasicAttackState = ENetherCrownBasicAttackState::ComboQueued;
	}
	else if (BasicAttackState == ENetherCrownBasicAttackState::CanAttack)
	{
		StartAttackBasic();
	}
}

void UNetherCrownBasicAttackComponent::StartAttackBasic()
{
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

void UNetherCrownBasicAttackComponent::PlayAttackSoundAndJumpToComboMontageSection(const FName* SectionName)
{
	if (!SectionName)
	{
		UE_LOG(LogTemp, Warning, TEXT("Montage section not found in %hs"), __FUNCTION__);
		return;
	}

	const USkeletalMeshComponent* OwnerCharacterMesh{ CachedCharacter ? CachedCharacter->GetMesh() : nullptr };
	UAnimInstance* AnimInstance{ OwnerCharacterMesh ? OwnerCharacterMesh->GetAnimInstance() : nullptr};
	UNetherCrownCharacterAnimInstance* NetherCrownCharacterAnimInstance{ Cast<UNetherCrownCharacterAnimInstance>(AnimInstance) };
	if (!ensureAlways(IsValid(NetherCrownCharacterAnimInstance)))
	{
		return;
	}

	BasicAttackState = ENetherCrownBasicAttackState::Attacking;

	if (!ensureAlways(IsValid(CachedBasicAttackMontage)))
	{
		return;
	}
	NetherCrownCharacterAnimInstance->Montage_Play(CachedBasicAttackMontage);
	NetherCrownCharacterAnimInstance->Montage_JumpToSection(*SectionName);
	//@NOTE : AnimMontage의 BlendOutTriggerTime을 0으로 Setting하여 Idle로 천천히 넘어가도록 제어하여 어색함을 없앰

	OnStopOrStartBasicAttackAnim.Broadcast(false);

	PlayBasicAttackSounds();
}

void UNetherCrownBasicAttackComponent::SetEquippedWeaponTraceEnable(const bool bEnable) const
{
	if (!ensureAlways(IsValid(CachedCharacter)))
	{
		return;
	}

	UNetherCrownEquipComponent* EquipComponent{ CachedCharacter->GetEquipComponent() };
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

void UNetherCrownBasicAttackComponent::Multicast_AutoTargetEnemy_Implementation()
{
	AutoTargetEnemy();
}

void UNetherCrownBasicAttackComponent::AutoTargetEnemy() const
{
	if (!ensureAlways(IsValid(CachedCharacter)))
	{
		return;
	}

	TArray<AActor*> OverlappedActors{};
	const TArray<TEnumAsByte<EObjectTypeQuery>>& EnemyTypes{ UEngineTypes::ConvertToObjectType(ECC_Enemy) };
	UKismetSystemLibrary::SphereOverlapActors(this, CachedCharacter->GetActorLocation(), AutoTargetingRadius, EnemyTypes,
		ANetherCrownEnemy::StaticClass(), TArray<AActor*>(), OverlappedActors);

	if (OverlappedActors.IsEmpty())
	{
		return;
	}

	AActor* AutoTargetActor{};
	for (AActor* OverlappedActor : OverlappedActors)
	{
		if (IsValid(AutoTargetActor))
		{
			const double DistanceToOverlappedActor{ UKismetMathLibrary::Distance2D(FVector2D(CachedCharacter->GetActorLocation() ), FVector2D(OverlappedActor->GetActorLocation())) };
			const double DistanceToAutoTargetActor{ UKismetMathLibrary::Distance2D(FVector2D(CachedCharacter->GetActorLocation() ), FVector2D(AutoTargetActor->GetActorLocation())) };

			if (DistanceToOverlappedActor < DistanceToAutoTargetActor)
			{
				AutoTargetActor = OverlappedActor;
			}
		}
		else
		{
			AutoTargetActor = OverlappedActor;
		}
	}

	const FRotator& AutoTargetRotation{ UKismetMathLibrary::FindLookAtRotation(CachedCharacter->GetActorLocation(), AutoTargetActor->GetActorLocation()) };
	CachedCharacter->SetActorRotation(AutoTargetRotation);
}

void UNetherCrownBasicAttackComponent::Multicast_PlayHitImpactEffect_Implementation(const FVector& HitLocation)
{
	SpawnHitImpactEffect(HitLocation);
}

void UNetherCrownBasicAttackComponent::Multicast_PlayHitImpactCameraShake_Implementation()
{
	PlayHitImpactCameraShake();
}

void UNetherCrownBasicAttackComponent::ApplyDamageInternal(AActor* HitEnemy) const
{
	if (!IsValid(CachedCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("CachedCharacter is invalid %hs"), __FUNCTION__);
		return;
	}

	UGameplayStatics::ApplyDamage(HitEnemy, CalculateBasicAttackDamage(), CachedCharacter->GetInstigatorController(), CachedCharacter, UDamageType::StaticClass());
}

void UNetherCrownBasicAttackComponent::HandleOnEquipWeapon(const bool bEquipWeapon)
{
	if (IsValid(CachedCharacter) && CachedCharacter->HasAuthority())
	{
		BasicAttackState = bEquipWeapon ? ENetherCrownBasicAttackState::CanAttack : ENetherCrownBasicAttackState::CannotAttack;
	}
}

int32 UNetherCrownBasicAttackComponent::CalculateBasicAttackDamage() const
{
	//@NOTE : Character의 Stat의 공격력, 현재 장착중인 Weapon의 공격력을 이용하여 데미지 계산을 한다
	if (!ensureAlways(IsValid(CachedCharacter)))
	{
		return 0;
	}

	const ANetherCrownPlayerState* OwnerPlayerState{ Cast<ANetherCrownPlayerState>(CachedCharacter->GetPlayerState()) };
	check(OwnerPlayerState);

	const UNetherCrownPlayerStatComponent* OwnerChracterStatComponent{ OwnerPlayerState->GetNetherCrownPlayerStatComponent() };
	if (!ensureAlways(IsValid(OwnerChracterStatComponent)))
	{
		return 0;
	}

	const FNetherCrownPlayerStatData& StatData{ OwnerChracterStatComponent->GetPlayerStatData() };
	const int32 AttackDamage{ StatData.AttackDamage };

	const UNetherCrownEquipComponent* EquipComponent{ CachedCharacter->GetEquipComponent() };
	if (!ensureAlways(IsValid(EquipComponent)))
	{
		return 0;
	}

	const UNetherCrownWeaponData* EquippedWeaponData{ EquipComponent->GetEquippedWeaponData() };
	const int32 WeaponAttackDamage{ EquippedWeaponData ? EquippedWeaponData->GetWeaponAttackDamage() : 0 };

	return AttackDamage + WeaponAttackDamage;
}

void UNetherCrownBasicAttackComponent::PlayHitImpactCameraShake() const
{
	if (!ensureAlways(IsValid(CachedCharacter)))
	{
		return;
	}

	if (!CachedCharacter->IsLocallyControlled())
	{
		return;
	}

	ANetherCrownPlayerController* OwnerPlayerController{ Cast<ANetherCrownPlayerController>(CachedCharacter->GetController()) };
	check(OwnerPlayerController);

	APlayerCameraManager* CameraManager{ OwnerPlayerController->PlayerCameraManager };
	check(CameraManager);

	CameraManager->StartCameraShake(ApplyDamageCameraShakeClass, 1.0f);
}

void UNetherCrownBasicAttackComponent::PlayBasicAttackSounds() const
{
	if (!ensureAlways(IsValid(CachedCharacter)))
	{
		return;
	}

	FNetherCrownUtilManager::PlaySound2DByGameplayTag(CachedCharacter, BasicAttackComponentTagData.BasicAttackGruntSoundTag);

	UNetherCrownEquipComponent* EquipComponent{ CachedCharacter->GetEquipComponent() };
	if (!ensureAlways(EquipComponent))
	{
		return;
	}

	const FGameplayTag& SwingWeaponSoundTag{ EquipComponent->GetEquippedWeaponTagData().WeaponSwingSound };
	FNetherCrownUtilManager::PlaySound2DByGameplayTag(CachedCharacter, SwingWeaponSoundTag);
}

void UNetherCrownBasicAttackComponent::SpawnHitImpactEffect(const FVector& HitLocation) const
{
	FTransform SpawnTransform{};
	SpawnTransform.SetLocation(HitLocation);
	SpawnTransform.SetRotation(FRotator::ZeroRotator.Quaternion());
	SpawnTransform.SetScale3D(FVector(1.0f));

	FNetherCrownUtilManager::SpawnNiagaraSystemByGameplayTag(CachedCharacter, BasicAttackComponentTagData.BasicAttackImpactEffectTag, SpawnTransform);
}

void UNetherCrownBasicAttackComponent::ApplyDamageToHitEnemy(AActor* HitEnemy, const FVector& HitLocation)
{
	//@NOTE : Only Server Called
	if (!ensureAlways(IsValid(CachedCharacter)))
	{
		return;
	}

	if (!IsValid(HitEnemy))
	{
		return;
	}

	Multicast_PlayHitImpactCameraShake();
	Multicast_PlayHitImpactEffect(HitLocation);

	ApplyDamageInternal(HitEnemy);
}

void UNetherCrownBasicAttackComponent::CalculateNextComboCount()
{
	const int32 MaxComboCount{ ComboMontageSectionMap.Num() };
	CurrentComboCount = CurrentComboCount + 1 > MaxComboCount ? 1 : ++CurrentComboCount;
}

void UNetherCrownBasicAttackComponent::HandleEnableComboWindow()
{
	//@NOTE : Do not use AnimNotifyState (Server<->Client duration issue)
	if (IsValid(CachedCharacter) && CachedCharacter->HasAuthority())
	{
		BasicAttackState = ENetherCrownBasicAttackState::CanQueueNextCombo;
	}
}

void UNetherCrownBasicAttackComponent::HandleDisableComboWindow()
{
	//@NOTE : Do not use AnimNotifyState (Server<->Client duration issue)
	if (IsValid(CachedCharacter) && CachedCharacter->HasAuthority())
	{
		if (BasicAttackState != ENetherCrownBasicAttackState::ComboQueued)
		{
			BasicAttackState = ENetherCrownBasicAttackState::CanAttack;

			SetEquippedWeaponTraceEnable(false);

			CurrentComboCount = 1;

			OnStopOrStartBasicAttackAnim.Broadcast(true);

			return;
		}

		CalculateNextComboCount();

		if (ComboMontageSectionMap.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("ComboMontageSectionMap is Empty in %hs"), __FUNCTION__);
			return;
		}

		BasicAttackState = ENetherCrownBasicAttackState::CanQueueNextCombo;

		const FName* CurrentComboMontageSectionName{ ComboMontageSectionMap.Find(CurrentComboCount) };
		Multicast_PlayAndJumpToComboMontageSection(*CurrentComboMontageSectionName);
	}

	SetEquippedWeaponTraceEnable(false);
}

void UNetherCrownBasicAttackComponent::HandleEnableHitTrace() const
{
	SetEquippedWeaponTraceEnable(true);
}

void UNetherCrownBasicAttackComponent::SetCanAttack(const bool InbCanAttack)
{
	BasicAttackState = InbCanAttack ? ENetherCrownBasicAttackState::CanAttack : ENetherCrownBasicAttackState::CannotAttack;
}
