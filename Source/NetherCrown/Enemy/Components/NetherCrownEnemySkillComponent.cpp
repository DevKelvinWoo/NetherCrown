// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemySkillComponent.h"

#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/NetherCrown.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Enemy/Components/NetherCrownEnemyActionControlComponent.h"
#include "NetherCrown/Enemy/EnemySkill/NetherCrownEnemySkillObject.h"

UNetherCrownEnemySkillComponent::UNetherCrownEnemySkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

bool UNetherCrownEnemySkillComponent::ActivateEnemySkill(const FGameplayTag& SkillTag)
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return false;
	}

	if (!CanActivateEnemySkill())
	{
		return false;
	}

	if (!SkillTag.IsValid() || EnemySkillObjectMap.IsEmpty())
	{
		return false;
	}

	UNetherCrownEnemySkillObject** FoundEnemySkillObjectPtr{ EnemySkillObjectMap.Find(SkillTag) };
	if (!FoundEnemySkillObjectPtr || !IsValid(*FoundEnemySkillObjectPtr))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("FoundSkillObjectPtr is Invalid in %hs"), __FUNCTION__);
		return false;
	}

	UNetherCrownEnemySkillObject* FoundEnemySkillObject{ *FoundEnemySkillObjectPtr };
	if (!ensureAlways(IsValid(FoundEnemySkillObject)))
	{
		return false;
	}

	FoundEnemySkillObject->ExecuteEnemySkillGameplay();
	Multicast_PlayEnemySkillCosmetics(FoundEnemySkillObject);

	return true;
}

UNetherCrownEnemySkillObject* UNetherCrownEnemySkillComponent::GetEnemySkillObject(const FGameplayTag& SkillTag) const
{
	if (!SkillTag.IsValid() || EnemySkillObjectMap.IsEmpty())
	{
		return nullptr;
	}

	UNetherCrownEnemySkillObject* const* FoundEnemySkillObjectPtr{ EnemySkillObjectMap.Find(SkillTag) };
	return FoundEnemySkillObjectPtr ? *FoundEnemySkillObjectPtr : nullptr;
}

bool UNetherCrownEnemySkillComponent::CanActivateEnemySkill() const
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return false;
	}

	const UNetherCrownEnemyActionControlComponent* EnemyActionControlComponent{ CachedOwnerEnemy->GetEnemyActionControlComponent() };
	if (!ensureAlways(IsValid(EnemyActionControlComponent)))
	{
		return false;
	}

	return EnemyActionControlComponent->CanUseSkill();
}

bool UNetherCrownEnemySkillComponent::IsEnemySkillCoolDown(const FGameplayTag& SkillTag) const
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return true;
	}

	const UNetherCrownEnemySkillObject* FoundEnemySkill{ GetEnemySkillObject(SkillTag) };
	if (!ensureAlways(IsValid(FoundEnemySkill)))
	{
		return true;
	}

	return FoundEnemySkill->IsEnemySkillCoolDown();
}

void UNetherCrownEnemySkillComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheInitData();
	ConstructEnemySkillObjects();
}

bool UNetherCrownEnemySkillComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	//@NOTE : Server에서만 호출됨
	bool bWroteSomething{ Super::ReplicateSubobjects(Channel, Bunch, RepFlags) };

	for (const auto& SkillPair : EnemySkillObjectMap)
	{
		if (IsValid(SkillPair.Value))
		{
			//@NOTE : UObject 자체로는 Channel이 없어 Replicate가 불가능하기에 생성된 Component의 Channel을 이욯하여 replicate한다
			//SkillObject의 Property들 (Replicated) 전부 replicate해준다
			bWroteSomething |= Channel->ReplicateSubobject(SkillPair.Value, *Bunch, *RepFlags);
		}
	}

	return bWroteSomething;
}

void UNetherCrownEnemySkillComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ReplicatedEnemySkillObjects);
}

void UNetherCrownEnemySkillComponent::CacheInitData()
{
	CachedOwnerEnemy = Cast<ANetherCrownEnemy>(GetOwner());
}

void UNetherCrownEnemySkillComponent::ConstructEnemySkillObjects()
{
	if (!ensureAlways(IsValid(CachedOwnerEnemy)) || !CachedOwnerEnemy->HasAuthority())
	{
		return;
	}

	for (const TSubclassOf<UNetherCrownEnemySkillObject>& SkillObjectClass : EnemySkillObjectClasses)
	{
		UNetherCrownEnemySkillObject* SkillObject{ NewObject<UNetherCrownEnemySkillObject>(this, SkillObjectClass) };
		if (!ensureAlways(IsValid(SkillObject)))
		{
			continue;
		}

		SkillObject->SetSkillOwnerEnemy(CachedOwnerEnemy);
		SkillObject->InitEnemySkillObject();

		EnemySkillObjectMap.Add(SkillObject->GetEnemySkillTag(), SkillObject);

		ReplicatedEnemySkillObjects.Add(SkillObject);
	}
}

void UNetherCrownEnemySkillComponent::OnRep_ReplicatedEnemySkillObjects()
{
	EnemySkillObjectMap.Empty();

	for (UNetherCrownEnemySkillObject* EnemySkillObject : ReplicatedEnemySkillObjects)
	{
		if (IsValid(EnemySkillObject))
		{
			EnemySkillObject->SetSkillOwnerEnemy(CachedOwnerEnemy);
			EnemySkillObject->InitEnemySkillObject();

			EnemySkillObjectMap.Add(EnemySkillObject->GetEnemySkillTag(), EnemySkillObject);
		}
	}
}

void UNetherCrownEnemySkillComponent::Multicast_PlayEnemySkillCosmetics_Implementation(UNetherCrownEnemySkillObject* FoundEnemySkillObject)
{
	if (!IsValid(FoundEnemySkillObject))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("FoundEnemySkillObject is Invalid in %hs"), __FUNCTION__);
		return;
	}

	FoundEnemySkillObject->PlayEnemySkillCosmetics();
}
