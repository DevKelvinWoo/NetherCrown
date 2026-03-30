// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownStatusEffectControlComponent.h"

#include "NetherCrown/NetherCrown.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "GameFramework/Character.h"

UNetherCrownStatusEffectControlComponent::UNetherCrownStatusEffectControlComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UNetherCrownStatusEffectControlComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedOwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!ensureAlways(IsValid(CachedOwnerCharacter)) || CachedOwnerCharacter->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	LoadStatusEffectCosmeticData();
}

void UNetherCrownStatusEffectControlComponent::LoadStatusEffectCosmeticData()
{
	if (StatusEffectCosmeticDataAssetSoft.IsNull() || CachedOwnerCharacter->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	const UNetherCrownStatusEffectCosmeticDataAsset* StatusEffectCosmeticDataAsset{ StatusEffectCosmeticDataAssetSoft.LoadSynchronous() };
	if (!ensureAlways(IsValid(StatusEffectCosmeticDataAsset)))
	{
		return;
	}

	StatusEffectCosmeticData = StatusEffectCosmeticDataAsset->GetStatusEffectCosmeticData();

	CachedStatusNiagaraSystemMap.Empty();
	for (const TPair<ENetherCrownCrowdControlType, TSoftObjectPtr<UNiagaraSystem>>& Pair : StatusEffectCosmeticData.StatusCosmeticMap)
	{
		if (Pair.Value.IsNull())
		{
			continue;
		}

		CachedStatusNiagaraSystemMap.Add(Pair.Key, Pair.Value.LoadSynchronous());
	}
}

void UNetherCrownStatusEffectControlComponent::SetHandledStatusNiagaraComponent(UNiagaraComponent* HandledStatusNiagaraComponent)
{
	HandledStatusNiagaraComponentWeak = MakeWeakObjectPtr(HandledStatusNiagaraComponent);
}

void UNetherCrownStatusEffectControlComponent::SetActiveStatusNiagaraSystem(const ENetherCrownCrowdControlType InCrowdControlType, const bool bEnable)
{
	if (!ensureAlways(IsValid(CachedOwnerCharacter)) || CachedOwnerCharacter->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	UNiagaraComponent* HandledStatusNiagaraComponent{ HandledStatusNiagaraComponentWeak.Get() };
	if (!IsValid(HandledStatusNiagaraComponent))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("HandledStatusNiagaraComponent is null %hs"), __FUNCTION__);
		return;
	}

	if (bEnable)
	{
		TObjectPtr<UNiagaraSystem>* CachedStatusNiagaraSystemPtr{ CachedStatusNiagaraSystemMap.Find(InCrowdControlType) };
		if (!CachedStatusNiagaraSystemPtr)
		{
			UE_LOG(LogNetherCrown, Warning, TEXT("CachedStatusNiagaraSystem is null %hs"), __FUNCTION__);
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
