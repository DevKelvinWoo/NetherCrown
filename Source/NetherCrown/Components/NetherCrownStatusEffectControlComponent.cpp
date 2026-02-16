// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownStatusEffectControlComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

UNetherCrownStatusEffectControlComponent::UNetherCrownStatusEffectControlComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UNetherCrownStatusEffectControlComponent::BeginPlay()
{
	Super::BeginPlay();

	for (const TPair<ENetherCrownCrowdControlType, TSoftObjectPtr<UNiagaraSystem>>& Pair : StatusNiagaraSystemMap)
	{
		const TSoftObjectPtr<UNiagaraSystem> SoftNiagaraSystemPtr{ Pair.Value };
		if (SoftNiagaraSystemPtr.IsNull())
		{
			continue;
		}

		CachedStatusNiagaraSystemMap.Add(Pair.Key, SoftNiagaraSystemPtr.LoadSynchronous());
	}
}

void UNetherCrownStatusEffectControlComponent::SetHandledStatusNiagaraComponent(UNiagaraComponent* HandledStatusNiagaraComponent)
{
	HandledStatusNiagaraComponentWeak = MakeWeakObjectPtr(HandledStatusNiagaraComponent);
}

void UNetherCrownStatusEffectControlComponent::SetActiveStatusNiagaraSystem(const ENetherCrownCrowdControlType InCrowdControlType, const bool bEnable)
{
	UNiagaraComponent* HandledStatusNiagaraComponent{ HandledStatusNiagaraComponentWeak.Get() };
	if (!IsValid(HandledStatusNiagaraComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("HandledStatusNiagaraComponent is null %hs"), __FUNCTION__);
	}

	if (bEnable)
	{
		TObjectPtr<UNiagaraSystem>* CachedStatusNiagaraSystemPtr{ CachedStatusNiagaraSystemMap.Find(InCrowdControlType) };
		if (!CachedStatusNiagaraSystemPtr)
		{
			UE_LOG(LogTemp, Warning, TEXT("CachedStatusNiagaraSystem is null %hs"), __FUNCTION__);
			return;
		}

		HandledStatusNiagaraComponent->SetAsset(*CachedStatusNiagaraSystemPtr);
		HandledStatusNiagaraComponent->Activate();
	}
	else
	{
		HandledStatusNiagaraComponent->SetAsset(nullptr);
		HandledStatusNiagaraComponent->Deactivate();
	}
}
