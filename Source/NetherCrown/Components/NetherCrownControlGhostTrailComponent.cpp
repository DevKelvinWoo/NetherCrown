// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownControlGhostTrailComponent.h"

#include "NetherCrown/NetherCrown.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"

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
	ACharacter* OwnerCharacter{ Cast<ACharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)) || OwnerCharacter->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	UNiagaraComponent* HandledGhostTrailNiagaraComponent{ HandledGhostTrailNiagaraComponentWeak.Get() };
	if (!IsValid(HandledGhostTrailNiagaraComponent))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("HandledGhostTrailNiagaraComponent is invalid in %hs"), __FUNCTION__);
		return;
	}

	if (!bActivate)
	{
		HandledGhostTrailNiagaraComponent->Deactivate();
		return;
	}

	if (!IsValid(InGhostTrailNiagaraSystem))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("InGhostTrailNiagaraSystem is invalid in %hs"), __FUNCTION__);
		return;
	}

	HandledGhostTrailNiagaraComponent->SetAsset(InGhostTrailNiagaraSystem);
	HandledGhostTrailNiagaraComponent->Activate();
}
