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

bool UNetherCrownSkillComponent::CanActivateSkill() const
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->HasAuthority())
	{
		return false;
	}

	const UNetherCrownBasicAttackComponent* BasicAttackComponent{ CachedCharacter->GetBasicAttackComponent() };
	if (!ensureAlways(IsValid(BasicAttackComponent)) || BasicAttackComponent->IsAttacking())
	{
		return false;
	}

	const UNetherCrownEquipComponent* EquipComponent{ CachedCharacter->GetEquipComponent() };
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

	for (const auto& SkillPair : SkillObjectMap)
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

	for (const auto& SkillPair : SkillObjectMap)
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
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->HasAuthority())
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
		SkillObjectMap.Add(SkillEnum, SkillObject);
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

	if (SkillObjectMap.IsEmpty())
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("SkillObjectMap is Empty in %hs"), __FUNCTION__);
		return;
	}

	if (SkillKeyEnum == ENetherCrownSkillKeyEnum::None)
	{
		return;
	}

	Client_SetActiveSkillKeyEnum(SkillKeyEnum);

	UNetherCrownSkillObject** FoundSkillObjectPtr{ SkillObjectMap.Find(SkillKeyEnum) };
	if (!IsValid(*FoundSkillObjectPtr))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("FoundSkillObjectPtr is Invalid in %hs"), __FUNCTION__);
		return;
	}

	UNetherCrownSkillObject* FoundSkillObject{ *FoundSkillObjectPtr };
	if (!FoundSkillObject->CanActiveSkill())
	{
		return;
	}

	FoundSkillObject->ExecuteSkillGameplay();
	Multicast_PlaySkillCosmetics(FoundSkillObject);
}

void UNetherCrownSkillComponent::Multicast_PlaySkillCosmetics_Implementation(UNetherCrownSkillObject* FoundSkillObject)
{
	if (!ensureAlways(IsValid(CachedCharacter)))
	{
		return;
	}

	if (!IsValid(FoundSkillObject))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("FoundSkillObject is Invalid in %hs"), __FUNCTION__);
		return;
	}

	FoundSkillObject->PlaySkillCosmetics();
}

void UNetherCrownSkillComponent::Client_SetActiveSkillKeyEnum_Implementation(const ENetherCrownSkillKeyEnum SkillKeyEnum)
{
	ActiveSkillKeyEnum = SkillKeyEnum;
}

void UNetherCrownSkillComponent::OnRep_ReplicatedSkillObjects()
{
	//@NOTE : Client에서 Server로부터 replicate된 ReplicatedSkillObjects를 이용하여 실제 사용한 SkillObjects를 채운다
	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };

	SkillObjectMap.Empty();

	for (UNetherCrownSkillObject* Obj : ReplicatedSkillObjects)
	{
		if (IsValid(Obj))
		{
			Obj->SetSkillOwnerCharacter(OwnerCharacter);
			Obj->InitSkillObject();

			SkillObjectMap.Add(Obj->GetSkillEnum(), Obj);
		}
	}
}
