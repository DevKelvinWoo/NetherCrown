// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DamageEvents.h"
#include "GameplayTagContainer.h"

class AActor;
class AController;
class UDamageType;

struct NETHERCROWN_API FNetherCrownDamageEvent : public FDamageEvent
{
	FGameplayTag HitImpactSoundTag{};
	FGameplayTag HitImpactEffectTag{};

	static float ApplyDamage(
		AActor* DamagedActor,
		float BaseDamage,
		AController* EventInstigator,
		AActor* DamageCauser,
		TSubclassOf<UDamageType> DamageTypeClass,
		const FGameplayTag& HitImpactSoundTag = FGameplayTag{},
		const FGameplayTag& HitImpactEffectTag = FGameplayTag{}
	);
};
