// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownDamageReceiverComponent.h"

UNetherCrownDamageReceiverComponent::UNetherCrownDamageReceiverComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

float UNetherCrownDamageReceiverComponent::HandleIncomingDamage(float DamageAmount, FDamageEvent const& DamageEvent, AActor* DamageCauser)
{
	//@TODO : Apply Damage Logic
	return DamageAmount;
}

void UNetherCrownDamageReceiverComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UNetherCrownDamageReceiverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
