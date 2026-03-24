// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownBasicAttackComponent.h"

#include "NetherCrown/NetherCrown.h"
#include "NetherCrownEquipComponent.h"
#include "NetherCrownPlayerStatComponent.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Character/NetherCrownPlayerController.h"
#include "NetherCrown/Character/AnimInstance/NetherCrownCharacterAnimInstance.h"
#include "NetherCrown/Data/NetherCrownWeaponData.h"
#include "NetherCrown/DamageTypes/NetherCrownPhysicalDamageType.h"
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
	LoadBasicAttackData();
	CacheBasicAttackMontage();

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
}

void UNetherCrownBasicAttackComponent::LoadBasicAttackData()
{
	if (BasicAttackDataAssetSoft.IsNull())
	{
		return;
	}

	const UNetherCrownBasicAttackDataAsset* BasicAttackDataAsset{ BasicAttackDataAssetSoft.LoadSynchronous() };
	if (!ensureAlways(IsValid(BasicAttackDataAsset)))
	{
		return;
	}

	BasicAttackData = BasicAttackDataAsset->GetBasicAttackData();
}

void UNetherCrownBasicAttackComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, BasicAttackState);
}

void UNetherCrownBasicAttackComponent::CacheBasicAttackMontage()
{
	if (!ensureAlways(IsValid(CachedCharacter)) || CachedCharacter->HasAuthority())
	{
		return;
	}

	if (BasicAttackData.BasicAttackAnimMontageSoft.IsNull())
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("BasicAttackAnimMontage is null %hs"), __FUNCTION__);
		return;
	}

	CachedBasicAttackMontage = BasicAttackData.BasicAttackAnimMontageSoft.LoadSynchronous();
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
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->HasAuthority())
	{
		return;
	}

	OnStopOrStartBasicAttackAnim.Broadcast(false);

	if (BasicAttackData.ComboDataMap.IsEmpty())
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("ComboDataMap is Empty in %hs"), __FUNCTION__);
		return;
	}

	CurrentComboCount = 1;

	const FNetherCrownBasicAttackComboData* FirstComboData{ BasicAttackData.ComboDataMap.Find(1) };
	if (!FirstComboData)
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("ComboData entry not found for combo 1 in %hs"), __FUNCTION__);
		return;
	}

	Multicast_PlayAndJumpToComboMontageSection(FirstComboData->ComboMontageSectionName);

	SetupBasicAttackTimers(CurrentComboCount);
}

void UNetherCrownBasicAttackComponent::Multicast_PlayAndJumpToComboMontageSection_Implementation(const FName& SectionName)
{
	if (!ensureAlways(IsValid(CachedCharacter)))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("CachedCharacter is invalid %hs"), __FUNCTION__);
		return;
	}

	if (CachedCharacter->HasAuthority())
	{
		BasicAttackState = ENetherCrownBasicAttackState::Attacking;
	}
	else
	{
		PlayAttackSoundAndJumpToComboMontageSection(&SectionName);
	}
}

void UNetherCrownBasicAttackComponent::PlayAttackSoundAndJumpToComboMontageSection(const FName* SectionName)
{
	if (!SectionName || !ensureAlways(IsValid(CachedCharacter)))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("Montage section not found or CachedCharacter is invalid in %hs"), __FUNCTION__);
		return;
	}

	if (CachedCharacter->HasAuthority())
	{
		return;
	}

	const USkeletalMeshComponent* OwnerCharacterMesh{ CachedCharacter->GetMesh() };
	UAnimInstance* AnimInstance{ OwnerCharacterMesh ? OwnerCharacterMesh->GetAnimInstance() : nullptr };
	UNetherCrownCharacterAnimInstance* NetherCrownCharacterAnimInstance{ Cast<UNetherCrownCharacterAnimInstance>(AnimInstance) };
	if (!ensureAlways(IsValid(NetherCrownCharacterAnimInstance)))
	{
		return;
	}

	if (!ensureAlways(IsValid(CachedBasicAttackMontage)))
	{
		return;
	}

	//@NOTE : AnimMontage의 BlendOutTriggerTime을 0으로 Setting하여 Idle로 천천히 넘어가도록 제어하여 어색함을 없앰
	NetherCrownCharacterAnimInstance->Montage_Play(CachedBasicAttackMontage);
	NetherCrownCharacterAnimInstance->Montage_JumpToSection(*SectionName);

	PlayBasicAttackSounds();
}

void UNetherCrownBasicAttackComponent::SetEquippedWeaponTraceEnable(const bool bEnable) const
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->HasAuthority())
	{
		return;
	}

	const UNetherCrownEquipComponent* EquipComponent{ CachedCharacter->GetEquipComponent() };
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
}

void UNetherCrownBasicAttackComponent::Client_InitWeaponTraceComponentSettings_Implementation()
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

	EquippedWeapon->InitWeaponTraceComponentSettings();
}

void UNetherCrownBasicAttackComponent::Multicast_AutoTargetEnemy_Implementation()
{
	if (IsValid(CachedCharacter) && CachedCharacter->IsLocallyControlled())
	{
		return;
	}

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
	UKismetSystemLibrary::SphereOverlapActors(this, CachedCharacter->GetActorLocation(), BasicAttackData.AutoTargetingRadius, EnemyTypes,
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
			const double DistanceToOverlappedActor{ UKismetMathLibrary::Distance2D(FVector2D(CachedCharacter->GetActorLocation()), FVector2D(OverlappedActor->GetActorLocation())) };
			const double DistanceToAutoTargetActor{ UKismetMathLibrary::Distance2D(FVector2D(CachedCharacter->GetActorLocation()), FVector2D(AutoTargetActor->GetActorLocation())) };

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
	if (!ensureAlways(IsValid(CachedCharacter)) || CachedCharacter->HasAuthority())
	{
		return;
	}

	SpawnHitImpactEffect(HitLocation);
}

void UNetherCrownBasicAttackComponent::Client_PlayHitImpactCameraShake_Implementation()
{
	PlayHitImpactCameraShake();
}

void UNetherCrownBasicAttackComponent::ApplyDamageInternal(AActor* HitEnemy) const
{
	if (!ensureAlways(IsValid(CachedCharacter)))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("CachedCharacter is invalid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedCharacter->HasAuthority())
	{
		return;
	}

	UGameplayStatics::ApplyDamage(HitEnemy, CalculateBasicAttackDamage(), CachedCharacter->GetInstigatorController(), CachedCharacter, UNetherCrownPhysicalDamageType::StaticClass());
}

void UNetherCrownBasicAttackComponent::HandleOnEquipWeapon(const bool bEquipWeapon)
{
	if (ensureAlways(IsValid(CachedCharacter)) && CachedCharacter->HasAuthority())
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

	if (!CachedCharacter->HasAuthority())
	{
		return 0;
	}

	const ANetherCrownPlayerState* OwnerPlayerState{ Cast<ANetherCrownPlayerState>(CachedCharacter->GetPlayerState()) };
	if (!ensureAlways(IsValid(OwnerPlayerState)))
	{
		return 0;
	}

	const UNetherCrownPlayerStatComponent* OwnerCharacterStatComponent{ OwnerPlayerState->GetNetherCrownPlayerStatComponent() };
	if (!ensureAlways(IsValid(OwnerCharacterStatComponent)))
	{
		return 0;
	}

	const FNetherCrownPlayerStat& StatData{ OwnerCharacterStatComponent->GetPlayerStatData() };
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

	CameraManager->StartCameraShake(BasicAttackData.ApplyDamageCameraShakeClass, 1.0f);
}

void UNetherCrownBasicAttackComponent::PlayBasicAttackSounds() const
{
	if (!ensureAlways(IsValid(CachedCharacter)))
	{
		return;
	}

	if (CachedCharacter->HasAuthority())
	{
		return;
	}

	FNetherCrownUtilManager::PlaySound2DByGameplayTag(CachedCharacter, BasicAttackData.BasicAttackTagData.BasicAttackGruntSoundTag);

	const UNetherCrownEquipComponent* EquipComponent{ CachedCharacter->GetEquipComponent() };
	if (!ensureAlways(EquipComponent))
	{
		return;
	}

	const ANetherCrownWeapon* EquippedWeapon{ EquipComponent->GetEquippedWeapon() };
	if (!ensureAlways(IsValid(EquippedWeapon)))
	{
		return;
	}

	const UNetherCrownWeaponData* EquippedWeaponData{ EquippedWeapon->GetWeaponData() };
	if (!ensureAlways(IsValid(EquippedWeaponData)))
	{
		return;
	}

	const FGameplayTag SwingWeaponSoundTag{ EquippedWeaponData->GetWeaponSwingSoundTag() };
	FNetherCrownUtilManager::PlaySound2DByGameplayTag(CachedCharacter, SwingWeaponSoundTag);
}

void UNetherCrownBasicAttackComponent::SpawnHitImpactEffect(const FVector& HitLocation) const
{
	if (!ensureAlways(IsValid(CachedCharacter)))
	{
		return;
	}

	if (CachedCharacter->HasAuthority())
	{
		return;
	}

	FTransform SpawnTransform{};
	SpawnTransform.SetLocation(HitLocation);
	SpawnTransform.SetRotation(FRotator::ZeroRotator.Quaternion());
	SpawnTransform.SetScale3D(FVector(1.0f));

	FNetherCrownUtilManager::SpawnNiagaraSystemByGameplayTag(CachedCharacter, BasicAttackData.BasicAttackTagData.BasicAttackImpactEffectTag, SpawnTransform);
}

void UNetherCrownBasicAttackComponent::ApplyDamageToHitEnemy(AActor* HitEnemy, const FVector& HitLocation)
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->HasAuthority())
	{
		return;
	}

	if (!IsValid(HitEnemy))
	{
		return;
	}

	Client_PlayHitImpactCameraShake();
	Multicast_PlayHitImpactEffect(HitLocation);

	ApplyDamageInternal(HitEnemy);
}

void UNetherCrownBasicAttackComponent::CalculateNextComboCount()
{
	const int32 MaxComboCount{ BasicAttackData.ComboDataMap.Num() };
	CurrentComboCount = CurrentComboCount >= MaxComboCount ? 1 : ++CurrentComboCount;
}

void UNetherCrownBasicAttackComponent::SetCanAttack(const bool InbCanAttack)
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->HasAuthority())
	{
		return;
	}

	BasicAttackState = InbCanAttack ? ENetherCrownBasicAttackState::CanAttack : ENetherCrownBasicAttackState::CannotAttack;
}

void UNetherCrownBasicAttackComponent::SetupBasicAttackTimers(const int32 ComboCount)
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->HasAuthority())
	{
		return;
	}

	const FNetherCrownBasicAttackComboData* ComboData{ BasicAttackData.ComboDataMap.Find(ComboCount) };
	if (!ComboData)
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("Timing entry not found for combo %d in %hs"), ComboCount, __FUNCTION__);
		return;
	}

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	FTimerManager& TimerManager{ World->GetTimerManager() };

	TimerManager.ClearTimer(ComboWindowOpenTimerHandle);
	TimerManager.ClearTimer(ComboWindowCloseTimerHandle);
	TimerManager.ClearTimer(AttackEndTimerHandle);
	TimerManager.ClearTimer(HitTraceEnableHandle);

	TimerManager.SetTimer(ComboWindowOpenTimerHandle, this, &ThisClass::ServerHandleComboWindowOpen, ComboData->ComboWindowOpenTime, false);
	TimerManager.SetTimer(ComboWindowCloseTimerHandle, this, &ThisClass::ServerHandleComboWindowClose, ComboData->ComboWindowCloseTime, false);
	TimerManager.SetTimer(AttackEndTimerHandle, this, &ThisClass::ServerHandleAttackEnd, ComboData->AttackEndTime, false);
	TimerManager.SetTimer(HitTraceEnableHandle, this, &ThisClass::ServerHandleHitTraceEnable, ComboData->HitTraceEnableTime, false);
}

void UNetherCrownBasicAttackComponent::ServerHandleComboWindowOpen()
{
	BasicAttackState = ENetherCrownBasicAttackState::CanQueueNextCombo;
}

void UNetherCrownBasicAttackComponent::ServerHandleComboWindowClose()
{
	if (BasicAttackState != ENetherCrownBasicAttackState::ComboQueued)
	{
		BasicAttackState = ENetherCrownBasicAttackState::CanAttack;
		CurrentComboCount = 1;
		return;
	}

	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AttackEndTimerHandle);
	}

	OnStopOrStartBasicAttackAnim.Broadcast(false);

	CalculateNextComboCount();

	if (BasicAttackData.ComboDataMap.IsEmpty())
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("ComboDataMap is Empty in %hs"), __FUNCTION__);
		BasicAttackState = ENetherCrownBasicAttackState::CanAttack;
		CurrentComboCount = 1;
		return;
	}

	const FNetherCrownBasicAttackComboData* NextComboData{ BasicAttackData.ComboDataMap.Find(CurrentComboCount) };
	if (!NextComboData)
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("ComboDataMap entry not found for combo %d in %hs"), CurrentComboCount, __FUNCTION__);
		BasicAttackState = ENetherCrownBasicAttackState::CanAttack;
		CurrentComboCount = 1;
		return;
	}

	Multicast_PlayAndJumpToComboMontageSection(NextComboData->ComboMontageSectionName);

	SetupBasicAttackTimers(CurrentComboCount);
}

void UNetherCrownBasicAttackComponent::ServerHandleAttackEnd()
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->HasAuthority())
	{
		return;
	}

	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ComboWindowOpenTimerHandle);
		World->GetTimerManager().ClearTimer(ComboWindowCloseTimerHandle);
		World->GetTimerManager().ClearTimer(AttackEndTimerHandle);
	}

	CurrentComboCount = 1;

	SetEquippedWeaponTraceEnable(false);

	OnStopOrStartBasicAttackAnim.Broadcast(true);
}

void UNetherCrownBasicAttackComponent::ServerHandleHitTraceEnable()
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->HasAuthority())
	{
		return;
	}

	Client_InitWeaponTraceComponentSettings();

	SetEquippedWeaponTraceEnable(true);
}
