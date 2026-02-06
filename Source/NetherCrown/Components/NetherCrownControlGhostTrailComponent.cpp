// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownControlGhostTrailComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

UNetherCrownControlGhostTrailComponent::UNetherCrownControlGhostTrailComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UNetherCrownControlGhostTrailComponent::SetHandledGhostTrailNiagaraComponent(UNiagaraComponent* InGhostTrailNiagaraComponent)
{
	HandledGhostTrailNiagaraComponentWeak = MakeWeakObjectPtr(InGhostTrailNiagaraComponent);
}

void UNetherCrownControlGhostTrailComponent::ActivateGhostTrail(const bool bActivate, UNiagaraSystem* InGhostTrailNiagaraSystem/*nullptr*/) const
{
	UNiagaraComponent* HandledGhostTrailNiagaraComponent{ HandledGhostTrailNiagaraComponentWeak.Get() };
	if (!IsValid(HandledGhostTrailNiagaraComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("HandledGhostTrailNiagaraComponent is invalid in %hs"), __FUNCTION__);
		return;
	}

	if (!bActivate)
	{
		HandledGhostTrailNiagaraComponent->Deactivate();
		return;
	}

	if (!IsValid(InGhostTrailNiagaraSystem))
	{
		UE_LOG(LogTemp, Warning, TEXT("InGhostTrailNiagaraSystem is invalid in %hs"), __FUNCTION__);
		return;
	}

	HandledGhostTrailNiagaraComponent->SetAsset(InGhostTrailNiagaraSystem);
	HandledGhostTrailNiagaraComponent->Activate();
}
