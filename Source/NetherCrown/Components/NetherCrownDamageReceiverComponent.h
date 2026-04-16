// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrownDamageReceiverComponent.generated.h"


class ANetherCrownCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownDamageReceiverComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownDamageReceiverComponent();

	float HandleIncomingDamage(float DamageAmount, FDamageEvent const& DamageEvent, AActor* DamageCauser);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void CacheOwnerCharacter();

	float CalculateFinalDamage(float DamageAmount, FDamageEvent const& DamageEvent, const AActor* DamageCauser) const;
	void ApplyFinalDamage(float FinalDamage);

	int32 GetArmorStat(const bool bIsPhysicalDamage) const;

	bool IsDead() const;

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownCharacter> CachedOwnerCharacter{};
};
