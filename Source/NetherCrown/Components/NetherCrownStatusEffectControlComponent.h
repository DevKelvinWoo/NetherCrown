// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrown/Data/NetherCrownStatusEffectCosmeticData.h"
#include "NetherCrownStatusEffectControlComponent.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownStatusEffectControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownStatusEffectControlComponent();

	void SetHandledStatusNiagaraComponent(UNiagaraComponent* HandledStatusNiagaraComponent);
	void SetActiveStatusNiagaraSystem(const bool bEnable, const ENetherCrownCrowdControlType InCrowdControlType = ENetherCrownCrowdControlType::NONE);

protected:
	virtual void BeginPlay() override;

private:
	void LoadStatusEffectCosmeticData();

	UPROPERTY(Transient)
	FNetherCrownStatusEffectCosmeticData StatusEffectCosmeticData{};

	UPROPERTY(EditDefaultsOnly, Category = "StatusEffectCosmeticDataAsset")
	TSoftObjectPtr<UNetherCrownStatusEffectCosmeticDataAsset> StatusEffectCosmeticDataAssetSoft{};

	UPROPERTY(Transient)
	TMap<ENetherCrownCrowdControlType, TObjectPtr<UNiagaraSystem>> CachedStatusNiagaraSystemMap{};

	TWeakObjectPtr<UNiagaraComponent> HandledStatusNiagaraComponentWeak{};

	UPROPERTY(Transient)
	TObjectPtr<ACharacter> CachedOwnerCharacter{};
};
