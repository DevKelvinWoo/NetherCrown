// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownSkillComponent.h"

UNetherCrownSkillComponent::UNetherCrownSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UNetherCrownSkillComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UNetherCrownSkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
