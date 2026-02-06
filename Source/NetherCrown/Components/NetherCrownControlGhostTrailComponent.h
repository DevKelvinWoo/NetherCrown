// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrownControlGhostTrailComponent.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownControlGhostTrailComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownControlGhostTrailComponent();

	void SetHandledGhostTrailNiagaraComponent(UNiagaraComponent* InGhostTrailNiagaraComponent);

	void ActivateGhostTrail(const bool bActivate, UNiagaraSystem* InGhostTrailNiagaraSystem = nullptr) const;

private:
	TWeakObjectPtr<UNiagaraComponent> HandledGhostTrailNiagaraComponentWeak{};
};
