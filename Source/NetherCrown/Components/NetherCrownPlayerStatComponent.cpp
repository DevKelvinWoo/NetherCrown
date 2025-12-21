// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownPlayerStatComponent.h"

UNetherCrownPlayerStatComponent::UNetherCrownPlayerStatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UNetherCrownPlayerStatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UNetherCrownPlayerStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
