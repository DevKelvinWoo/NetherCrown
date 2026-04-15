// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Data/NetherCrownEnemyBasicAttackData.h"
#include "NetherCrownEnemyBasicAttackComponent.generated.h"

class UAnimMontage;

class ANetherCrownEnemyWeapon;
class ANetherCrownEnemy;

UENUM()
enum class ENetherCrownEnemyBasicAttackState : uint8
{
	Attacking,
	NotAttacking
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownEnemyBasicAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownEnemyBasicAttackComponent();

	void RequestEnemyAttack();

	void SetHandledEnemyWeapon(ANetherCrownEnemyWeapon* InWeapon);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void SetupBasicAttackTimer();

	void EnableHitTrace();
	void DisableHitTrace();
	void EndAttack();

	void DetectHit();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayBasicAttackMontage();

	void LoadEnemyBasicAttackData();
	void CacheBasicAttackAnimMontage();

	FVector GetWeaponTraceSocketLocation() const;

	UPROPERTY(Transient)
	FNetherCrownEnemyBasicAttackData EnemyBasicAttackData{};

	UPROPERTY(EditDefaultsOnly, Category = "EnemyBasicAttackDataAsset")
	TSoftObjectPtr<UNetherCrownEnemyBasicAttackDataAsset> EnemyBasicAttackDataAssetSoft{};

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CachedBasicAttackMontage{};

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownEnemy> CachedOwnerEnemy{};

	UPROPERTY(Replicated)
	bool bEnableHitTrace{ false };

	UPROPERTY(Replicated)
	ENetherCrownEnemyBasicAttackState EnemyBasicAttackState{ ENetherCrownEnemyBasicAttackState::NotAttacking };

	UPROPERTY(Replicated)
	FVector LastEndLocation{};

	TWeakObjectPtr<ANetherCrownEnemyWeapon> HandledEnemyWeaponWeak{};

	FTimerHandle AttackEndTimerHandle{};
	FTimerHandle EnableHitTraceTimerHandle{};
	FTimerHandle DisableHitTraceTimerHandle{};
};
