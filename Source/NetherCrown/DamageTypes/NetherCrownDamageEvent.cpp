// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownDamageEvent.h"

#include "GameFramework/Actor.h"

float FNetherCrownDamageEvent::ApplyDamage(
	AActor* DamagedActor,
	const float BaseDamage,
	AController* EventInstigator,
	AActor* DamageCauser,
	const TSubclassOf<UDamageType> DamageTypeClass,
	const FGameplayTag& HitImpactSoundTag,
	const FGameplayTag& HitImpactEffectTag
)
{
	if (!IsValid(DamagedActor))
	{
		return 0.f;
	}

	if (!DamagedActor->HasAuthority())
	{
		return 0.f;
	}

	FNetherCrownDamageEvent DamageEvent{};
	DamageEvent.DamageTypeClass = DamageTypeClass;
	DamageEvent.HitImpactSoundTag = HitImpactSoundTag;
	DamageEvent.HitImpactEffectTag = HitImpactEffectTag;

	return DamagedActor->TakeDamage(BaseDamage, DamageEvent, EventInstigator, DamageCauser);
}
