// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NetherCrownEffectData.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FNetherCrownEffectData : public FTableRowBase
{
	GENERATED_BODY()

public:
	const FGameplayTag& GetEffectTag() const { return EffectTag; }
	TSoftObjectPtr<UNiagaraSystem> GetEffectNiagaraSystem() const { return EffectNiagaraSystem; }

private:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag EffectTag{};

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UNiagaraSystem> EffectNiagaraSystem{};
};
