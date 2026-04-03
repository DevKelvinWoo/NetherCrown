// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownSkillObject.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "NetherCrown/Character/AnimInstance/NetherCrownCharacterAnimInstance.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownBasicAttackComponent.h"
#include "NetherCrown/Components/NetherCrownControlPPComponent.h"
#include "NetherCrown/Components/NetherCrownCrowdControlComponent.h"
#include "NetherCrown/Components/NetherCrownEquipComponent.h"
#include "NetherCrown/Components/NetherCrownPlayerStatComponent.h"
#include "NetherCrown/Components/NetherCrownSkillComponent.h"
#include "NetherCrown/Data/NetherCrownSkillData.h"
#include "NetherCrown/Data/NetherCrownWeaponData.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/PlayerState/NetherCrownPlayerState.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"
#include "NetherCrown/Weapon/NetherCrownWeapon.h"

#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

void UNetherCrownSkillObject::SetSkillOwnerCharacter(ANetherCrownCharacter* SkillOwnerCharacter)
{
	SkillOwnerCharacterWeak = MakeWeakObjectPtr(SkillOwnerCharacter);
}

void UNetherCrownSkillObject::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, SkillTag);
	DOREPLIFETIME(ThisClass, SkillOwnerCharacterWeak);
	DOREPLIFETIME(ThisClass, SkillData);
	DOREPLIFETIME(ThisClass, bIsCoolDown);
}

ENetherCrownSkillKeyEnum UNetherCrownSkillObject::GetSkillEnum() const
{
	return SkillData.SkillKeyEnum;
}

const FNetherCrownSkillData& UNetherCrownSkillObject::GetSkillData() const
{
	return SkillData;
}

void UNetherCrownSkillObject::CacheSkillData()
{
	if (!SkillTag.IsValid())
	{
		return;
	}

	const UNetherCrownSkillDataAsset* SkillDataAsset{ FNetherCrownUtilManager::GetSkillDataAssetByGameplayTag(SkillTag) };
	if (!ensureAlways(IsValid(SkillDataAsset)))
	{
		return;
	}

	SkillData = SkillDataAsset->GetSkillData();
}

float UNetherCrownSkillObject::GetSkillHitTime() const
{
	return SkillData.SkillHitTime;
}

const TSoftObjectPtr<UAnimMontage>& UNetherCrownSkillObject::GetSkillAnimMontageSoft() const
{
	return SkillData.SkillAnimMontageSoft;
}

const FGameplayTag& UNetherCrownSkillObject::GetSkillHitImpactEffectTag() const
{
	return SkillData.SkillHitImpactEffectTag;
}

float UNetherCrownSkillObject::GetSkillStartTimeOffset() const
{
	return SkillData.SkillStartTimeOffset;
}

float UNetherCrownSkillObject::GetSkillEndTimeOffset() const
{
	return SkillData.SkillEndTimeOffset;
}

float UNetherCrownSkillObject::GetCharacterMovementFlyTimeOffset() const
{
	return SkillData.CharacterMovementFlyTimeOffset;
}

float UNetherCrownSkillObject::GetCharacterMovementWalkTimeOffset() const
{
	return SkillData.CharacterMovementWalkTimeOffset;
}

float UNetherCrownSkillObject::GetSkillAuraActiveTimeOffset() const
{
	return SkillData.SkillAuraActiveTimeOffset;
}

float UNetherCrownSkillObject::GetSkillAuraDeactivateTimeOffset() const
{
	return SkillData.SkillAuraDeactivateTimeOffset;
}

float UNetherCrownSkillObject::GetSkillMontageBeginSlowPlayRate() const
{
	return SkillData.SkillMontageBeginSlowPlayRate;
}

float UNetherCrownSkillObject::GetSkillMontageBeginSlowTimeOffset() const
{
	return SkillData.SkillMontageBeginSlowTimeOffset;
}

float UNetherCrownSkillObject::GetSkillMontageEndSlowPlayRate() const
{
	return SkillData.SkillMontageEndSlowPlayRate;
}

float UNetherCrownSkillObject::GetSkillMontageEndSlowTimeOffset() const
{
	return SkillData.SkillMontageEndSlowTimeOffset;
}

float UNetherCrownSkillObject::GetSkillCoolDownDecreaseOffset() const
{
	return SkillData.SkillCoolDownDecreaseOffset;
}

int32 UNetherCrownSkillObject::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	// Outer가 있으면 Outer의 callspace를 사용
	if (UObject* Outer = GetOuter())
	{
		return Outer->GetFunctionCallspace(Function, Stack);
	}
	return FunctionCallspace::Local;
}

bool UNetherCrownSkillObject::CallRemoteFunction(UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack)
{
	// Outer (Component)를 통해 RPC 라우팅
	if (AActor* Owner = GetTypedOuter<AActor>())
	{
		UNetDriver* NetDriver = Owner->GetNetDriver();
		if (NetDriver)
		{
			NetDriver->ProcessRemoteFunction(Owner, Function, Parms, OutParms, Stack, this);
			return true;
		}
	}
	return false;
}

void UNetherCrownSkillObject::Multicast_SpawnSkillImpactEffect_Implementation(const ANetherCrownEnemy* TargetEnemy) const
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || SkillOwnerCharacter->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	PlaySkillHitImpactEffect(TargetEnemy);
}

void UNetherCrownSkillObject::ApplyCrowdControlToTarget(const ANetherCrownEnemy* TargetEnemy, const ENetherCrownCrowdControlType InCrowdControlType, const float CrowdControlDuration)
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || !SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	if (!ensureAlways(IsValid(TargetEnemy)))
	{
		return;
	}

	UNetherCrownCrowdControlComponent* EnemyCrowdControlComponent{ TargetEnemy->GetCrowdControlComponent() };
	if (!ensureAlways(EnemyCrowdControlComponent))
	{
		return;
	}

	EnemyCrowdControlComponent->ApplyCrowdControl(InCrowdControlType, CrowdControlDuration);
}

void UNetherCrownSkillObject::PlaySkillHitImpactEffect(const ANetherCrownEnemy* TargetEnemy) const
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || SkillOwnerCharacter->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (GetSkillHitImpactEffectTag().IsValid())
	{
		FNetherCrownUtilManager::SpawnNiagaraSystemByGameplayTag(this, GetSkillHitImpactEffectTag(), TargetEnemy->GetActorTransform());
	}
}

int32 UNetherCrownSkillObject::CalculateSkillDamage() const
{
	//Character의 Stat + Weapon의 Damage + SkillDamage
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || !SkillOwnerCharacter->HasAuthority())
	{
		return 0;
	}

	const ANetherCrownPlayerState* PlayerState{ Cast<ANetherCrownPlayerState>(SkillOwnerCharacter->GetPlayerState()) };
	if (!ensureAlways(IsValid(PlayerState)))
	{
		return 0;
	}

	const UNetherCrownPlayerStatComponent* PlayerStatComponent{ PlayerState->GetNetherCrownPlayerStatComponent() };
	if (!ensureAlways(IsValid(PlayerStatComponent)))
	{
		return 0;
	}

	const int32 PlayerStatAttackDamage{ PlayerStatComponent->GetPlayerStatData().AttackDamage };
	const int32 SkillDamage{ SkillData.SkillDamage };

	UNetherCrownEquipComponent* EquipComponent{ SkillOwnerCharacter->GetEquipComponent() };
	if (!ensureAlways(IsValid(EquipComponent)))
	{
		return 0;
	}

	const UNetherCrownWeaponData* EquippedWeaponData{ EquipComponent->GetEquippedWeaponData() };
	if (!ensureAlways(IsValid(EquippedWeaponData)))
	{
		return 0;
	}

	const int32 EquippedWeaponDamage { EquippedWeaponData->GetWeaponAttackDamage() };
	const int32 ResultSkillDamage{ EquippedWeaponDamage + SkillDamage + PlayerStatAttackDamage };
	return ResultSkillDamage;
}

void UNetherCrownSkillObject::ApplyPostProcess(const ENetherCrownPPType PPType, float Duration, const bool bEndTimerAutomatic) const
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || !SkillOwnerCharacter->IsLocallyControlled())
	{
		return;
	}

	UNetherCrownControlPPComponent* ControlPPComponent{ SkillOwnerCharacter->GetControlPPComponent() };
	if (!ensureAlways(IsValid(ControlPPComponent)))
	{
		return;
	}

	ControlPPComponent->ApplyPostProcess(PPType, Duration, bEndTimerAutomatic);
}

void UNetherCrownSkillObject::SetupSkillStateTimer()
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || !SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	FTimerManager& TimerManager{ World->GetTimerManager() };
	TimerManager.ClearTimer(SkillStartTimerHandle);
	TimerManager.ClearTimer(SkillEndTimerHandle);

	if (GetSkillStartTimeOffset() >= 0.f)
	{
		TimerManager.SetTimer(SkillStartTimerHandle, this, &ThisClass::StartSkillState, GetSkillStartTimeOffset(), false);
	}

	if (GetSkillEndTimeOffset() >= 0.f)
	{
		TimerManager.SetTimer(SkillEndTimerHandle, this, &ThisClass::EndSkillState, GetSkillEndTimeOffset(), false);
	}
}

void UNetherCrownSkillObject::SetupSkillMovementModeTimer()
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || !SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	FTimerManager& TimerManager{ World->GetTimerManager() };
	TimerManager.ClearTimer(CharacterMovementFlyTimerHandle);
	TimerManager.ClearTimer(CharacterMovementWalkTimerHandle);

	if (GetCharacterMovementFlyTimeOffset() >= 0.f)
	{
		TimerManager.SetTimer(CharacterMovementFlyTimerHandle, this, &ThisClass::SetCharacterMovementFly, GetCharacterMovementFlyTimeOffset(), false);
	}

	if (GetCharacterMovementWalkTimeOffset() >= 0.f)
	{
		TimerManager.SetTimer(CharacterMovementWalkTimerHandle, this, &ThisClass::SetCharacterMovementWalk, GetCharacterMovementWalkTimeOffset(), false);
	}
}

void UNetherCrownSkillObject::SetupSkillSlowTimer()
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || SkillOwnerCharacter->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	FTimerManager& TimerManager{ World->GetTimerManager() };
	TimerManager.ClearTimer(SkillAnimationSlowStartTimerHandle);
	TimerManager.ClearTimer(SkillAnimationSlowEndTimerHandle);

	if (GetSkillMontageBeginSlowTimeOffset() >= 0.f)
	{
		TimerManager.SetTimer(SkillAnimationSlowStartTimerHandle, this, &ThisClass::MakeSkillAnimationSlowly, GetSkillMontageBeginSlowTimeOffset(), false);
	}

	if (GetSkillMontageEndSlowTimeOffset() >= 0.f)
	{
		TimerManager.SetTimer(SkillAnimationSlowEndTimerHandle, this, &ThisClass::RestoreSkillAnimationPlayRate, GetSkillMontageEndSlowTimeOffset(), false);
	}
}

void UNetherCrownSkillObject::SetupSkillWeaponAuraTimer()
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	FTimerManager& TimerManager{ World->GetTimerManager() };
	TimerManager.ClearTimer(SkillWeaponAuraActiveTimerHandle);
	TimerManager.ClearTimer(SkillWeaponAuraDeactivateTimerHandle);

	if (GetSkillAuraActiveTimeOffset() >= 0.f)
	{
		TimerManager.SetTimer(SkillWeaponAuraActiveTimerHandle, this, &ThisClass::ActiveSkillWeaponAura, GetSkillAuraActiveTimeOffset(), false);
	}

	if (GetSkillAuraDeactivateTimeOffset() >= 0.f)
	{
		TimerManager.SetTimer(SkillWeaponAuraDeactivateTimerHandle, this, &ThisClass::DeactivateSkillWeaponAura, GetSkillAuraDeactivateTimeOffset(), false);
	}
}

void UNetherCrownSkillObject::StartSkillState()
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || !SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	UNetherCrownBasicAttackComponent* BasicAttackComponent{ SkillOwnerCharacter->GetBasicAttackComponent() };
	if (!ensureAlways(IsValid(BasicAttackComponent)))
	{
		return;
	}

	BasicAttackComponent->SetCanAttack(false);

	UNetherCrownSkillComponent* SkillComponent{ SkillOwnerCharacter->GetSkillComponent() };
	if (!ensureAlways(IsValid(SkillComponent)))
	{
		return;
	}

	SkillComponent->GetOnStopOrStartSkill().Broadcast(false);
}

void UNetherCrownSkillObject::EndSkillState()
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || !SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	UNetherCrownBasicAttackComponent* BasicAttackComponent{ SkillOwnerCharacter->GetBasicAttackComponent() };
	if (!ensureAlways(IsValid(BasicAttackComponent)))
	{
		return;
	}

	BasicAttackComponent->SetCanAttack(true);

	UNetherCrownSkillComponent* SkillComponent{ SkillOwnerCharacter->GetSkillComponent() };
	if (!ensureAlways(IsValid(SkillComponent)))
	{
		return;
	}

	SkillComponent->GetOnStopOrStartSkill().Broadcast(true);
}

void UNetherCrownSkillObject::MakeSkillAnimationSlowly()
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || SkillOwnerCharacter->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	SetSkillMontageSlowPlayRate(GetSkillMontageBeginSlowPlayRate());
}

void UNetherCrownSkillObject::RestoreSkillAnimationPlayRate()
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || SkillOwnerCharacter->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	SetSkillMontageSlowPlayRate(GetSkillMontageEndSlowPlayRate());
}

void UNetherCrownSkillObject::SetCharacterMovementFly()
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || !SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	UCharacterMovementComponent* CharacterMovementComponent{ SkillOwnerCharacter->GetCharacterMovement() };
	if (!ensureAlways(IsValid(CharacterMovementComponent)))
	{
		return;
	}

	CharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Flying);
}

void UNetherCrownSkillObject::SetCharacterMovementWalk()
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || !SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	UCharacterMovementComponent* CharacterMovementComponent{ SkillOwnerCharacter->GetCharacterMovement() };
	if (!ensureAlways(IsValid(CharacterMovementComponent)))
	{
		return;
	}

	CharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
}

void UNetherCrownSkillObject::SetSkillWeaponAura(const bool bIsActivate)
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	UNetherCrownEquipComponent* EquipComponent{ SkillOwnerCharacter->GetEquipComponent() };
	if (!ensureAlways(IsValid(EquipComponent)))
	{
		return;
	}

	ANetherCrownWeapon* EquippedWeapon{ EquipComponent->GetEquippedWeapon() };
	if (!ensureAlways(IsValid(EquippedWeapon)))
	{
		return;
	}

	EquippedWeapon->Multicast_ActiveWeaponSkillAuraNiagara(bIsActivate, GetSkillEnum());
}

void UNetherCrownSkillObject::ActiveSkillWeaponAura()
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	SetSkillWeaponAura(true);
}

void UNetherCrownSkillObject::DeactivateSkillWeaponAura()
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	SetSkillWeaponAura(false);
}

void UNetherCrownSkillObject::SpendMP()
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || !SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	const ANetherCrownPlayerState* OwnerPlayerState{ Cast<ANetherCrownPlayerState>(SkillOwnerCharacter->GetPlayerState()) };
	if (!ensureAlways(IsValid(OwnerPlayerState)))
	{
		return;
	}

	UNetherCrownPlayerStatComponent* OwnerCharacterStatComponent{ OwnerPlayerState->GetNetherCrownPlayerStatComponent() };
	if (!ensureAlways(IsValid(OwnerCharacterStatComponent)))
	{
		return;
	}

	OwnerCharacterStatComponent->ModifyMP(-SkillData.SkillMPCost);
}

void UNetherCrownSkillObject::InitSkillObject()
{
	CacheSkillData();

	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	const USkeletalMeshComponent* SkeletalMeshComponent{ SkillOwnerCharacter->GetMesh() };
	UNetherCrownCharacterAnimInstance* NetherCrownCharacterAnimInstance{};
	NetherCrownCharacterAnimInstance = SkeletalMeshComponent ? Cast<UNetherCrownCharacterAnimInstance>(SkeletalMeshComponent->GetAnimInstance()) : nullptr;
	if (!ensureAlways(IsValid(NetherCrownCharacterAnimInstance)))
	{
		return;
	}

	CachedSkillAnimMontage = GetSkillAnimMontageSoft().IsNull() ? nullptr : GetSkillAnimMontageSoft().LoadSynchronous();
}

void UNetherCrownSkillObject::PlaySkillCosmetics()
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	const USkeletalMeshComponent* SkeletalMeshComponent{ SkillOwnerCharacter->GetMesh() };
	UNetherCrownCharacterAnimInstance* NetherCrownCharacterAnimInstance{};
	NetherCrownCharacterAnimInstance = SkeletalMeshComponent ? Cast<UNetherCrownCharacterAnimInstance>(SkeletalMeshComponent->GetAnimInstance()) : nullptr;
	if (!ensureAlways(IsValid(NetherCrownCharacterAnimInstance)) || !IsValid(CachedSkillAnimMontage))
	{
		return;
	}

	if (SkillOwnerCharacter->GetNetMode() == NM_DedicatedServer && !CachedSkillAnimMontage->HasRootMotion())
	{
		return;
	}

	NetherCrownCharacterAnimInstance->Montage_Play(CachedSkillAnimMontage);
}

void UNetherCrownSkillObject::ExecuteSkillGameplay()
{
	SpendMP();
	StartSkillCoolDownTimer();
	SetupSkillStateTimer();
}

void UNetherCrownSkillObject::SetSkillMontageSlowPlayRate(float InPlayRate) const
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || SkillOwnerCharacter->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	USkeletalMeshComponent* SkeletalMeshComponent{ SkillOwnerCharacter->GetMesh() };
	UNetherCrownCharacterAnimInstance* NetherCrownCharacterAnimInstance{};
	NetherCrownCharacterAnimInstance = SkeletalMeshComponent ? Cast<UNetherCrownCharacterAnimInstance>(SkeletalMeshComponent->GetAnimInstance()) : nullptr;
	if (!ensureAlways(IsValid(NetherCrownCharacterAnimInstance)))
	{
		return;
	}

	UAnimMontage* SkillAnimMontage{ GetSkillAnimMontageSoft().IsNull() ? nullptr : GetSkillAnimMontageSoft().LoadSynchronous() };
	if (!ensureAlways(IsValid(SkillAnimMontage)))
	{
		return;
	}

	NetherCrownCharacterAnimInstance->Montage_SetPlayRate(SkillAnimMontage, InPlayRate);
}

void UNetherCrownSkillObject::StartSkillCoolDownTimer()
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(SkillOwnerCharacter) || !SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	SkillCoolDownAccumulator = SkillData.SkillCooldown;
	Client_SkillCoolDownModify(1.f);

	World->GetTimerManager().SetTimer(SkillCoolDownTimerHandle, this, &ThisClass::StopSkillCoolDownTimer, GetSkillCoolDownDecreaseOffset(), true);

	bIsCoolDown = true;
}

void UNetherCrownSkillObject::StopSkillCoolDownTimer()
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(SkillOwnerCharacter) || !SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	SkillCoolDownAccumulator -= GetSkillCoolDownDecreaseOffset();

	Client_SkillCoolDownModify(SkillCoolDownAccumulator / SkillData.SkillCooldown);

	if (SkillCoolDownAccumulator <= 0.f)
	{
		Client_SkillCoolDownModify(0.f);

		const UWorld* World{ GetWorld() };
		if (!ensureAlways(IsValid(World)))
		{
			return;
		}

		World->GetTimerManager().ClearTimer(SkillCoolDownTimerHandle);
		bIsCoolDown = false;
	}
}

void UNetherCrownSkillObject::Client_SkillCoolDownModify_Implementation(const float CoolDownRatio)
{
	OnSkillCoolDownModified.Broadcast(CoolDownRatio, GetSkillEnum());
}
