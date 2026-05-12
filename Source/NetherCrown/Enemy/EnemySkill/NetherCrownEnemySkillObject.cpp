// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownEnemySkillObject.h"

#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownEnemyStatComponent.h"
#include "NetherCrown/DamageTypes/NetherCrownDamageEvent.h"
#include "NetherCrown/DamageTypes/NetherCrownMagicDamageType.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Enemy/AnimInstance/NetherCrownEnemyAnimInstance.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

void UNetherCrownEnemySkillObject::SetSkillOwnerEnemy(ANetherCrownEnemy* SkillOwnerEnemy)
{
	SkillOwnerEnemyWeak = MakeWeakObjectPtr(SkillOwnerEnemy);
}

void UNetherCrownEnemySkillObject::InitEnemySkillObject()
{
	CacheSkillData();

	if (!EnemySkillData.EnemySkillAnimMontageSoft.IsNull())
	{
		CachedEnemySkillAnimMontage = EnemySkillData.EnemySkillAnimMontageSoft.LoadSynchronous();
	}
}

void UNetherCrownEnemySkillObject::PlayEnemySkillCosmetics()
{
	const ANetherCrownEnemy* SkillOwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerEnemy)))
	{
		return;
	}

	const USkeletalMeshComponent* SkeletalMeshComponent{ SkillOwnerEnemy->GetMesh() };
	UNetherCrownEnemyAnimInstance* NetherCrownEnemyAnimInstance{};
	NetherCrownEnemyAnimInstance = SkeletalMeshComponent ? Cast<UNetherCrownEnemyAnimInstance>(SkeletalMeshComponent->GetAnimInstance()) : nullptr;
	if (!ensureAlways(IsValid(NetherCrownEnemyAnimInstance)) || !IsValid(CachedEnemySkillAnimMontage))
	{
		return;
	}

	if (SkillOwnerEnemy->GetNetMode() == NM_DedicatedServer && !CachedEnemySkillAnimMontage->HasRootMotion())
	{
		return;
	}

	NetherCrownEnemyAnimInstance->Montage_Play(CachedEnemySkillAnimMontage);
}

void UNetherCrownEnemySkillObject::ExecuteEnemySkillGameplay()
{
	StartSkillCoolDownTimer();
	StartEnemySkillDurationTimer();
}

void UNetherCrownEnemySkillObject::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EnemySkillTag);
	DOREPLIFETIME(ThisClass, SkillOwnerEnemyWeak);
	DOREPLIFETIME(ThisClass, bIsCoolDown);
}

int32 UNetherCrownEnemySkillObject::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	// Outer가 있으면 Outer의 callspace를 사용
	if (UObject* Outer = GetOuter())
	{
		return Outer->GetFunctionCallspace(Function, Stack);
	}
	return FunctionCallspace::Local;
}

bool UNetherCrownEnemySkillObject::CallRemoteFunction(UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack)
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

void UNetherCrownEnemySkillObject::CacheSkillData()
{
	if (!EnemySkillTag.IsValid())
	{
		return;
	}

	const UNetherCrownEnemySkillDataAsset* EnemySkillDataAsset{ FNetherCrownUtilManager::GetEnemySkillDataAssetByGameplayTag(EnemySkillTag) };
	if (!ensureAlways(IsValid(EnemySkillDataAsset)))
	{
		return;
	}

	EnemySkillData = EnemySkillDataAsset->GetEnemySkillData();
}

void UNetherCrownEnemySkillObject::StartSkillCoolDownTimer()
{
	const ANetherCrownEnemy* SkillOwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerEnemy)) || !SkillOwnerEnemy->HasAuthority())
	{
		return;
	}

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	bIsCoolDown = true;

	FTimerManager& TimerManager{ World->GetTimerManager() };
	TimerManager.SetTimer(SkillCoolDownTimerHandle, this, &ThisClass::StopSkillCoolDownTimer, EnemySkillData.EnemySkillCoolDown, false);
}

void UNetherCrownEnemySkillObject::StopSkillCoolDownTimer()
{
	const ANetherCrownEnemy* SkillOwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerEnemy)) || !SkillOwnerEnemy->HasAuthority())
	{
		return;
	}

	bIsCoolDown = false;
}

void UNetherCrownEnemySkillObject::StartEnemySkillDurationTimer()
{
	const ANetherCrownEnemy* SkillOwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerEnemy)) || !SkillOwnerEnemy->HasAuthority())
	{
		return;
	}

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	FTimerManager& TimerManager{ World->GetTimerManager() };
	TimerManager.ClearTimer(TimerHandle_EnemySkillFinished);
	TimerManager.SetTimer(TimerHandle_EnemySkillFinished, this, &ThisClass::FinishEnemySkill, EnemySkillData.EnemySkillDuration, false);
}

void UNetherCrownEnemySkillObject::FinishEnemySkill()
{
	const ANetherCrownEnemy* SkillOwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!IsValid(SkillOwnerEnemy) || !SkillOwnerEnemy->HasAuthority())
	{
		return;
	}

	if (const UWorld* World{ GetWorld() })
	{
		World->GetTimerManager().ClearTimer(TimerHandle_EnemySkillFinished);
	}

	OnEnemySkillFinished.Broadcast();
}

int32 UNetherCrownEnemySkillObject::CalculateEnemyMagicSkillDamage() const
{
	const ANetherCrownEnemy* SkillOwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerEnemy)) || !SkillOwnerEnemy->HasAuthority())
	{
		return 0;
	}

	const UNetherCrownEnemyStatComponent* EnemyStatComponent{ SkillOwnerEnemy->GetEnemyStatComponent() };
	if (!ensureAlways(IsValid(EnemyStatComponent)))
	{
		return 0;
	}

	return EnemyStatComponent->GetEnemyStatData().MagicPower + EnemySkillData.EnemySkillDamage;
}

void UNetherCrownEnemySkillObject::ApplyEnemyMagicSkillDamage(ANetherCrownCharacter* TargetCharacter) const
{
	ApplyEnemySkillDamage(TargetCharacter, CalculateEnemyMagicSkillDamage(), UNetherCrownMagicDamageType::StaticClass());
}

void UNetherCrownEnemySkillObject::ApplyEnemySkillDamage(ANetherCrownCharacter* TargetCharacter, const int32 DamageAmount, const TSubclassOf<UDamageType> DamageTypeClass) const
{
	ANetherCrownEnemy* SkillOwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerEnemy)) || !SkillOwnerEnemy->HasAuthority())
	{
		return;
	}

	if (!ensureAlways(IsValid(TargetCharacter)))
	{
		return;
	}

	FNetherCrownDamageEvent::ApplyDamage(
		TargetCharacter,
		DamageAmount,
		SkillOwnerEnemy->GetInstigatorController(),
		SkillOwnerEnemy,
		DamageTypeClass,
		EnemySkillData.EnemySkillTagData.HitImpactSoundTag,
		EnemySkillData.EnemySkillTagData.HitImpactEffectTag
	);
}
