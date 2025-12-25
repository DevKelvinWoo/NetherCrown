// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownSkillComponent.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Skill/NetherCrownSkillObject.h"

UNetherCrownSkillComponent::UNetherCrownSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void UNetherCrownSkillComponent::ActiveSkill(const ENetherCrownSkillKeyEnum SkillKeyEnum)
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

	FoundSkillObject->ActiveSkill();
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

void UNetherCrownSkillComponent::ConstructSkillObjects()
{
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

		const ENetherCrownSkillKeyEnum SkillEnum{ SkillObject->GetSkillEnum() };
		SkillObjects.Add(SkillEnum, SkillObject);
	}
}
