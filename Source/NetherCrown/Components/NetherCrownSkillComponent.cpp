// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownSkillComponent.h"

#include "NetherCrown/Skill/NetherCrownSkillObject.h"

UNetherCrownSkillComponent::UNetherCrownSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UNetherCrownSkillComponent::ActiveSkill(const ENetherCrownSkillEnum SkillEnum)
{
	if (SkillObjects.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillObjects is Empty in %hs"), __FUNCTION__);
		return;
	}

	UNetherCrownSkillObject* FoundSkillObject{ *SkillObjects.Find(SkillEnum) };
	if (!ensureAlways(IsValid(FoundSkillObject)))
	{
		return;
	}

	FoundSkillObject->ActiveSkill();
}

void UNetherCrownSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	for (TSubclassOf<UNetherCrownSkillObject> SkillObjectClass : SkillObjectClasses)
	{
		UNetherCrownSkillObject* SkillObject{ NewObject<UNetherCrownSkillObject>(SkillObjectClass) };
		if (!ensureAlways(IsValid(SkillObject)))
		{
			continue;
		}

		const ENetherCrownSkillEnum SkillEnum{ SkillObject->GetSkillEnum() };
		SkillObjects.Add(SkillEnum, SkillObject);
	}
}

void UNetherCrownSkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
