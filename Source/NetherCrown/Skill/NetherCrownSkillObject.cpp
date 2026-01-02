// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownSkillObject.h"

#include "Net/UnrealNetwork.h"
#include "NetherCrown/Character/AnimInstance/NetherCrownCharacterAnimInstance.h"
#include "TimerManager.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

void UNetherCrownSkillObject::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, SkillKeyEnum);
	DOREPLIFETIME(ThisClass, SkillOwnerCharacterWeak);
	DOREPLIFETIME(ThisClass, SkillMontageBeginSlowPlayRate);
	DOREPLIFETIME(ThisClass, SkillMontageEndSlowPlayRate);
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

void UNetherCrownSkillObject::Multicast_PlayEnemyHitSoundAndPlayImpactEffect_Implementation(const ANetherCrownEnemy* TargetEnemy) const
{
	PlayEnemyHitSound(TargetEnemy);
	PlaySkillHitImpactEffect(TargetEnemy);
}

void UNetherCrownSkillObject::ApplyKnockBackToTarget(ACharacter* TargetCharacter, const FVector& KnockBackVector)
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

	if (!ensureAlways(IsValid(TargetCharacter)))
	{
		UE_LOG(LogTemp, Warning, TEXT("TargetCharacter is valid %hs"), __FUNCTION__);
		return;
	}

	TargetCharacter->LaunchCharacter(KnockBackVector, true, true);

	//@TODO : CC기 구조 설계가 되면 여기서 CC Enum값을 셋팅한다
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
