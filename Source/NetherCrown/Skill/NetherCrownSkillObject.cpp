// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownSkillObject.h"

#include "NetherCrown/Character/AnimInstance/NetherCrownCharacterAnimInstance.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownCrowdControlComponent.h"
#include "NetherCrown/Components/NetherCrownEquipComponent.h"
#include "NetherCrown/Components/NetherCrownPlayerStatComponent.h"
#include "NetherCrown/Data/NetherCrownWeaponData.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/PlayerState/NetherCrownPlayerState.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

void UNetherCrownSkillObject::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, SkillKeyEnum);
	DOREPLIFETIME(ThisClass, SkillOwnerCharacterWeak);
	DOREPLIFETIME(ThisClass, SkillMontageBeginSlowPlayRate);
	DOREPLIFETIME(ThisClass, SkillMontageEndSlowPlayRate);
	DOREPLIFETIME(ThisClass, SkillData);
	DOREPLIFETIME(ThisClass, bCanActiveSkill);
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
	PlaySkillHitImpactEffect(TargetEnemy);
}

void UNetherCrownSkillObject::ApplyCrowdControlToTarget(ANetherCrownEnemy* TargetEnemy, const ENetherCrownCrowdControlType InCrowdControlType, const float CrowdControlDuration)
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	if (!SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	if (!ensureAlways(IsValid(TargetEnemy)))
	{
		UE_LOG(LogTemp, Warning, TEXT("TargetCharacter is valid %hs"), __FUNCTION__);
		return;
	}

	TargetEnemy->ApplyCrowdControl(InCrowdControlType, CrowdControlDuration);
}

void UNetherCrownSkillObject::PlayEnemyHitSound(const ANetherCrownEnemy* TargetEnemy) const
{
	if (!ensureAlways(IsValid(TargetEnemy)))
	{
		return;
	}

	TargetEnemy->PlayTakeDamageSound();
}

void UNetherCrownSkillObject::PlaySkillHitImpactEffect(const ANetherCrownEnemy* TargetEnemy) const
{
	if (SkillEffectTagData.SkillHitImpactEffectTag.IsValid())
	{
		FNetherCrownUtilManager::SpawnNiagaraSystemByGameplayTag(this, SkillEffectTagData.SkillHitImpactEffectTag, TargetEnemy->GetActorTransform());
	}
}

int32 UNetherCrownSkillObject::CalculatePhysicalSkillDamage() const
{
	//Character의 Stat + Weapon의 Damage + SkillDamage
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
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
	if (!ensureAlways(IsValid(NetherCrownCharacterAnimInstance)))
	{
		return;
	}

	UAnimMontage* SkillAnimMontage{ SkillAnimMontageSoft.LoadSynchronous() };
	if (!ensureAlways(IsValid(SkillAnimMontage)))
	{
		return;
	}

	NetherCrownCharacterAnimInstance->Montage_Play(SkillAnimMontage);
}

void UNetherCrownSkillObject::ExecuteSkillGameplay()
{
	StartSkillCoolDownTimer();
}

void UNetherCrownSkillObject::SetSkillMontageSlowPlayRate(float InPlayRate) const
{
	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	USkeletalMeshComponent* SkeletalMeshComponent{ SkillOwnerCharacter ? SkillOwnerCharacter->GetMesh() : nullptr };
	UNetherCrownCharacterAnimInstance* NetherCrownCharacterAnimInstance{};
	NetherCrownCharacterAnimInstance = SkeletalMeshComponent ? Cast<UNetherCrownCharacterAnimInstance>(SkeletalMeshComponent->GetAnimInstance()) : nullptr;
	if (!ensureAlways(IsValid(NetherCrownCharacterAnimInstance)))
	{
		return;
	}

	UAnimMontage* SkillAnimMontage{ SkillAnimMontageSoft.LoadSynchronous() };
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
	check(World);

	FTimerHandle SkillCoolDownTimerHandle{};
	World->GetTimerManager().SetTimer(SkillCoolDownTimerHandle, this, &ThisClass::StopSkillCoolDownTimer, SkillData.SkillCooldown, false);

	bCanActiveSkill = false;
}

void UNetherCrownSkillObject::StopSkillCoolDownTimer()
{
	bCanActiveSkill = true;
}
