// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "NetherCrown/Data/UNetherCrownEnemyDamageAndDeathCosmeticData.h"
#include "NetherCrownEnemyDamageReceiverComponent.generated.h"


class ANetherCrownEnemy;
class UNetherCrownDamageType;
class UMaterialInstanceDynamic;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownEnemyDamageReceiverComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownEnemyDamageReceiverComponent();

	float HandleIncomingDamage(float DamageAmount, FDamageEvent const& DamageEvent, const AActor* DamageCauser);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	float CalculateFinalDamage(float DamageAmount, FDamageEvent const& DamageEvent, const AActor* DamageCauser) const;

	void ApplyFinalDamage(float FinalDamage);

	bool IsDead() const;
	void HandleEnemyDead();
	void HandleDeathTimer();

	int32 GetWeaponPenetration(const bool bIsPhysicalDamage, const AActor* DamageCauser) const;
	int32 GetArmorStat(const bool bIsPhysicalDamage) const;

	void LoadEnemyDamageCosmeticData();
	void CacheDeathMaterialInstances();
	void BindTimelineFunctions();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayTakeDamageSound();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayDeathSound();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayTakeDamageAnimation(const bool bIsCriticalDamage);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_StartDeathDissolve();

	UFUNCTION()
	void ApplyDeadMaterialParam(float FloatCurveValue);

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownEnemy> CachedOwnerEnemy{};

	UPROPERTY(Transient)
	FUNetherCrownEnemyDamageCosmeticData EnemyDamageCosmeticData{};

	UPROPERTY(Transient)
	FNetherCrownEnemyDeathCosmeticData EnemyDeathCosmeticData{};

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CachedTakeDamageAnimMontage{};

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CachedTakeCriticalDamageAnimMontage{};

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> CachedDeathMaterialParamCurveFloat{};

	UPROPERTY(Transient)
	TArray<TObjectPtr<UMaterialInstanceDynamic>> CachedDeathMaterialInstances{};

	UPROPERTY(EditDefaultsOnly, Category = "EnemyDamageAndDeathCosmeticDataAsset")
	TSoftObjectPtr<UNetherCrownEnemyDamageAndDeathCosmeticDataAsset> EnemyDamageAndDeathCosmeticDataAssetSoft{};

	FTimerHandle HandleDestroyTimerHandle{};

	FTimeline DeathMaterialParamTimeline{};
};
