// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrown/Data/NetherCrownEnemyDamageCosmeticData.h"
#include "NetherCrownEnemyDamageReceiverComponent.generated.h"


class ANetherCrownEnemy;
class UNetherCrownDamageType;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownEnemyDamageReceiverComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownEnemyDamageReceiverComponent();

	float HandleIncomingDamage(float DamageAmount, FDamageEvent const& DamageEvent, const AActor* DamageCauser);

protected:
	virtual void BeginPlay() override;

private:
	float CalculateFinalDamage(float DamageAmount, FDamageEvent const& DamageEvent, const AActor* DamageCauser) const;

	void ApplyFinalDamage(float FinalDamage);

	bool IsDead() const;

	int32 GetWeaponPenetration(const bool bIsPhysicalDamage, const AActor* DamageCauser) const;
	int32 GetArmorStat(const bool bIsPhysicalDamage) const;

	void LoadEnemyDamageCosmeticData();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayTakeDamageSound();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayTakeDamageAnimation(const ENetherCrownCrowdControlType InCrowdControlType);

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownEnemy> CachedOwnerEnemy{};

	UPROPERTY(Transient)
	FNetherCrownEnemyDamageCosmeticData EnemyDamageCosmeticData{};

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CachedTakeDamageAnimMontage{};

	UPROPERTY(EditDefaultsOnly, Category = "EnemyDamageCosmeticDataAsset")
	TSoftObjectPtr<UNetherCrownEnemyDamageCosmeticDataAsset> EnemyDamageCosmeticDataAssetSoft{};
};
