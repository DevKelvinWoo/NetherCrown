// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrownEnemyBasicAttackComponent.generated.h"

class ANetherCrownEnemy;
class UAnimMontage;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownEnemyBasicAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownEnemyBasicAttackComponent();

	void RequestEnemyAttack();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayBasicAttackMontage();

	void CacheBasicAttackAnimMontage();

	void TestFunc();

	UPROPERTY(EditDefaultsOnly, Category = "AnimMontage")
	TSoftObjectPtr<UAnimMontage> BasicAttackMontageSoft{};

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CachedBasicAttackMontage{};

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownEnemy> CachedOwnerEnemy{};

	bool bCanAttack{ true };

	FTimerHandle TestTimerHandle{};
};
