// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownSkillComponent.h"

#include "AssetDefinitionAssetInfo.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Skill/NetherCrownSkillObject.h"

UNetherCrownSkillComponent::UNetherCrownSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void UNetherCrownSkillComponent::ActiveSkill(const ENetherCrownSkillKeyEnum SkillKeyEnum)
{
	Server_ActiveSkill(SkillKeyEnum);
}

void UNetherCrownSkillComponent::SetActiveSkillSlowPlayRate(const bool bBeginSlow)
{
	if (SkillObjects.IsEmpty())
	{
		return;
	}

	UNetherCrownSkillObject* FoundSkillObject{ *SkillObjects.Find(ActiveSkillKeyEnum) };
	if (!IsValid(FoundSkillObject))
	{
		UE_LOG(LogTemp, Warning, TEXT("ActiveSkillKeyEnum is Invalid in %hs"), __FUNCTION__);
		return;
	}

	const float SkillPlayRate{ bBeginSlow ? FoundSkillObject->GetSkillMontageBeginSlowPlayRate() : FoundSkillObject->GetSkillMontageEndSlowPlayRate() };
	FoundSkillObject->SetSkillMontageSlowPlayRate(SkillPlayRate);
}

void UNetherCrownSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	ConstructSkillObjects();
}

void UNetherCrownSkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
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

	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)))
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

		SkillObject->SetSkillOwnerCharacter(OwnerCharacter);
		SkillObject->InitSkillObject();

		//@NOTE : Server에서만 SkillObjects, ReplicatedSkillObjects를 구축한다
		//이때 ReplicatedSkillObjects는 Replicate가 되기 때문에 OnRep_ReplicatedSKillObjects가 호출된다 (클라에서)
		const ENetherCrownSkillKeyEnum SkillEnum{ SkillObject->GetSkillEnum() };
		SkillObjects.Add(SkillEnum, SkillObject);
		ReplicatedSkillObjects.Add(SkillObject);
	}
}

void UNetherCrownSkillComponent::Server_ActiveSkill_Implementation(const ENetherCrownSkillKeyEnum SkillKeyEnum)
{
	if (SkillObjects.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillObjects is Empty in %hs"), __FUNCTION__);
		return;
	}

	UNetherCrownSkillObject* FoundSkillObject{ *SkillObjects.Find(SkillKeyEnum) };
	if (!ensureAlways(IsValid(FoundSkillObject)))
	{
		return;
	}

	ActiveSkillKeyEnum = SkillKeyEnum;

	FoundSkillObject->ExecuteSkillGameplay();
	Multicast_PlaySkillCosmetics(FoundSkillObject);
}

void UNetherCrownSkillComponent::Multicast_PlaySkillCosmetics_Implementation(UNetherCrownSkillObject* FoundSkillObject)
{
	FoundSkillObject->PlaySkillCosmetics();
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
