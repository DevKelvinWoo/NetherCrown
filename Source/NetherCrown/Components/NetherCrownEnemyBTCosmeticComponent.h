// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrownEnemyBTCosmeticComponent.generated.h"

class UNetherCrownEnemyBTCosmeticData;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownEnemyBTCosmeticComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownEnemyBTCosmeticComponent();

	void PlayEnemyFoundCosmetic();

protected:
	virtual void BeginPlay() override;

private:
	void CacheEnemyBTCosmeticData();
	void CacheOwnerAnimInstance();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayEnemyFoundCosmetic();

	UPROPERTY(Transient)
	TObjectPtr<UNetherCrownEnemyBTCosmeticData> CachedEnemyBTCosmeticData{};

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CachedEnemyFoundAnimation{};

	UPROPERTY(Transient)
	TObjectPtr<UAnimInstance> CachedOwnerAnimInstance{};

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UNetherCrownEnemyBTCosmeticData> EnemyBTCosmeticDataSoft{};
};
