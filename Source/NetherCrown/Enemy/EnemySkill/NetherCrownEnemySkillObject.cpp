// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownEnemySkillObject.h"

#include "Net/UnrealNetwork.h"
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

	if (!EnemySkillData.SkillAnimMontageSoft.IsNull())
	{
		CachedEnemySkillAnimMontage = EnemySkillData.SkillAnimMontageSoft.LoadSynchronous();
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
}

void UNetherCrownEnemySkillObject::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EnemySkillTag);
	DOREPLIFETIME(ThisClass, SkillOwnerEnemyWeak);
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
