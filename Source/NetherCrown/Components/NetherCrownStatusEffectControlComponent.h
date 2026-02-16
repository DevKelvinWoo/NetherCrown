// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrownStatusEffectControlComponent.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

enum class ENetherCrownCrowdControlType : uint8;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownStatusEffectControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownStatusEffectControlComponent();

	void SetHandledStatusNiagaraComponent(UNiagaraComponent* HandledStatusNiagaraComponent);
	void SetActiveStatusNiagaraSystem(const ENetherCrownCrowdControlType InCrowdControlType, const bool bEnable);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly)
	TMap<ENetherCrownCrowdControlType, TSoftObjectPtr<UNiagaraSystem>> StatusNiagaraSystemMap{};

	UPROPERTY()
	TMap<ENetherCrownCrowdControlType, TObjectPtr<UNiagaraSystem>> CachedStatusNiagaraSystemMap{};

	TWeakObjectPtr<UNiagaraComponent> HandledStatusNiagaraComponentWeak{};
};
