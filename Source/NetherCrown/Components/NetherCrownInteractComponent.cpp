// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownInteractComponent.h"

#include "Net/UnrealNetwork.h"
#include "NetherCrown/Interact/NetherCrownInteract.h"

UNetherCrownInteractComponent::UNetherCrownInteractComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UNetherCrownInteractComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UNetherCrownInteractComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TargetInteractActor);
}

void UNetherCrownInteractComponent::Server_InteractToTarget_Implementation()
{
	auto* InteractActor{ Cast<INetherCrownInteract>(TargetInteractActor) };
	if (!InteractActor)
	{
		return;
	}

	InteractActor->Interact();
}

void UNetherCrownInteractComponent::SetTargetInteractActor(AActor* InTargetInteractActor)
{
	TargetInteractActor = InTargetInteractActor;
}

void UNetherCrownInteractComponent::InteractToTarget()
{
	Server_InteractToTarget();
}
