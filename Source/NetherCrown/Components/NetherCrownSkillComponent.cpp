// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownSkillComponent.h"

#include "NetherCrown/NetherCrown.h"
#include "NetherCrownBasicAttackComponent.h"
#include "NetherCrownEquipComponent.h"
#include "Engine/ActorChannel.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Skill/NetherCrownSkillObject.h"
#include "NetherCrown/Skill/NetherCrownSkillSkyFallSlash.h"
#include "NetherCrown/Weapon/NetherCrownWeapon.h"

UNetherCrownSkillComponent::UNetherCrownSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void UNetherCrownSkillComponent::ActivateSkill(const ENetherCrownSkillKeyEnum SkillKeyEnum)
{
	Server_ActivateSkill(SkillKeyEnum);
}

void UNetherCrownSkillComponent::SetActiveSkillSlowPlayRate(const bool bBeginSlow)
{
	if (!CachedCharacter->HasAuthority())
	{
		return;
	}

	Multicast_SetActiveSkillSlowPlayRate(bBeginSlow);
}

bool UNetherCrownSkillComponent::CanActivateSkill() const
{
	const UNetherCrownBasicAttackComponent* BasicAttackComponent{ CachedCharacter ? CachedCharacter->GetBasicAttackComponent() : nullptr };
	if (!ensureAlways(IsValid(BasicAttackComponent)) || BasicAttackComponent->IsAttacking())
	{
		return false;
	}

	const UNetherCrownEquipComponent* EquipComponent{ CachedCharacter ? CachedCharacter->GetEquipComponent() : nullptr };
	if (!ensureAlways(IsValid(EquipComponent)) || !IsValid(EquipComponent->GetEquippedWeapon()))
	{
		return false;
	}

	const UPawnMovementComponent* MovementComponent{ CachedCharacter->GetMovementComponent() };
	if (!ensureAlways(IsValid(MovementComponent)) || MovementComponent->IsFalling())
	{
		return false;
	}

	return true;
}

void UNetherCrownSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheInitData();
	ConstructSkillObjects();
}

void UNetherCrownSkillComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (const auto& SkillPair : SkillObjects)
	{
		UNetherCrownSkillObject* SkillObject{ SkillPair.Value };
		if (!IsValid(SkillObject))
		{
			continue;
		}

		SkillObject->TickFloatTimeline(DeltaTime);
	}
}

bool UNetherCrownSkillComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	//@NOTE : Server에서만 호출됨
	bool bWroteSomething{ Super::ReplicateSubobjects(Channel, Bunch, RepFlags) };

	for (const auto& SkillPair : SkillObjects)
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

void UNetherCrownSkillComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ReplicatedSkillObjects);
	DOREPLIFETIME(ThisClass, ActiveSkillKeyEnum);
}

void UNetherCrownSkillComponent::ConstructSkillObjects()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	if (!ensureAlways(IsValid(CachedCharacter)))
	{
		return;
	}

	for (TSubclassOf<UNetherCrownSkillObject> SkillObjectClass : SkillObjectClasses)
	{
		UNetherCrownSkillObject* SkillObject{ NewObject<UNetherCrownSkillObject>(this, SkillObjectClass) };
		if (!ensureAlways(IsValid(SkillObject)))
		{
			continue;
		}

		SkillObject->SetSkillOwnerCharacter(CachedCharacter);
		SkillObject->InitSkillObject();

		//@NOTE : Server에서만 SkillObjects, ReplicatedSkillObjects를 구축한다
		//이때 ReplicatedSkillObjects는 Replicate가 되기 때문에 OnRep_ReplicatedSKillObjects가 호출된다 (클라에서)
		const ENetherCrownSkillKeyEnum SkillEnum{ SkillObject->GetSkillEnum() };
		SkillObjects.Add(SkillEnum, SkillObject);
		ReplicatedSkillObjects.Add(SkillObject);
	}
}

void UNetherCrownSkillComponent::CacheInitData()
{
	CachedCharacter = Cast<ANetherCrownCharacter>(GetOwner());
}

void UNetherCrownSkillComponent::Server_ActivateSkill_Implementation(const ENetherCrownSkillKeyEnum SkillKeyEnum)
{
	if (!CanActivateSkill())
	{
		return;
	}

	if (SkillObjects.IsEmpty())
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("SkillObjects is Empty in %hs"), __FUNCTION__);
		return;
	}

	UNetherCrownSkillObject* FoundSkillObject{ *SkillObjects.Find(SkillKeyEnum) };
	if (!ensureAlways(IsValid(FoundSkillObject)))
	{
		return;
	}

	if (!FoundSkillObject->CanActiveSkill())
	{
		return;
	}

	Multicast_SetActiveSkillKeyEnum(SkillKeyEnum);

	FoundSkillObject->ExecuteSkillGameplay();
	Multicast_PlaySkillCosmetics(FoundSkillObject);
}

void UNetherCrownSkillComponent::Multicast_PlaySkillCosmetics_Implementation(UNetherCrownSkillObject* FoundSkillObject)
{
	FoundSkillObject->PlaySkillCosmetics();
}

void UNetherCrownSkillComponent::Multicast_SetActiveSkillKeyEnum_Implementation(const ENetherCrownSkillKeyEnum SkillKeyEnum)
{
	ActiveSkillKeyEnum = SkillKeyEnum;
}

void UNetherCrownSkillComponent::Multicast_SetActiveSkillSlowPlayRate_Implementation(const bool bBeginSlow)
{
	if (SkillObjects.IsEmpty())
	{
		return;
	}

	const UNetherCrownSkillObject* FoundSkillObject{ *SkillObjects.Find(ActiveSkillKeyEnum) };
	if (!IsValid(FoundSkillObject))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("ActiveSkillKeyEnum is Invalid in %hs"), __FUNCTION__);
		return;
	}

	const float SkillPlayRate{ bBeginSlow ? FoundSkillObject->GetSkillMontageBeginSlowPlayRate() : FoundSkillObject->GetSkillMontageEndSlowPlayRate() };
	FoundSkillObject->SetSkillMontageSlowPlayRate(SkillPlayRate);
}

void UNetherCrownSkillComponent::OnRep_ReplicatedSkillObjects()
{
	//@NOTE : Client에서 Server로부터 replicate된 ReplicatedSkillObjects를 이용하여 실제 사용한 SkillObjects를 채운다
	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)))
	{
		return;
	}

	SkillObjects.Empty();

	for (UNetherCrownSkillObject* Obj : ReplicatedSkillObjects)
	{
		if (IsValid(Obj))
		{
			Obj->SetSkillOwnerCharacter(OwnerCharacter);
			Obj->InitSkillObject();

			SkillObjects.Add(Obj->GetSkillEnum(), Obj);
		}
	}
}
