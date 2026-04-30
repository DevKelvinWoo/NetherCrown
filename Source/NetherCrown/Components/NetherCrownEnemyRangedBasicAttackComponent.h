// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrownEnemyRangedBasicAttackComponent.generated.h"

class ANetherCrownEnemyMagicProjectile;
class ANetherCrownCharacter;
class UNetherCrownEnemyRangedBasicAttackDataAsset;
class UNetherCrownEnemyProjectileDataAsset;
class ANetherCrownEnemy;

UENUM()
enum class ENetherCrownEnemyRangedBasicAttackState
{
	Attacking,
	NotAttacking
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownEnemyRangedBasicAttackComponent : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE(FOnEnemyRangedBasicAttackFinished);

public:
	UNetherCrownEnemyRangedBasicAttackComponent();

	void RequestEnemyRangedAttack();

	FOnEnemyRangedBasicAttackFinished& GetOnEnemyRangedBasicAttackFinished() { return OnEnemyRangedBasicAttackFinished; }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void CacheEnemyRangedBasicAttackData();
	void CacheInitData();

	void SpawnRangedBasicAttackProjectile(const FName& StartFireSocketName);
	FVector GetProjectileSpawnLocation(const FName& StartFireSocketName) const;

	void StartRangedBasicAttack();
	void PlayAttackMontageAndSpawnProjectile();

	void EndRangedBasicAttack();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayRangedBasicAttackAnim(const FName& ComboSectionName);

	int32 GetEnemyMagicAttackDamage() const;

	void HandleOnMagicProjectileHit(ANetherCrownCharacter* HitCharacter, ANetherCrownEnemyMagicProjectile* DamageCauserProjectile);

	UPROPERTY(Transient)
	TObjectPtr<UNetherCrownEnemyRangedBasicAttackDataAsset> CachedEnemyRangedBasicAttackDataAsset{};

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TSoftObjectPtr<UNetherCrownEnemyRangedBasicAttackDataAsset> EnemyRangedBasicAttackDataAssetSoft{};

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownEnemy> CachedOwnerEnemy{};

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CachedBasicAttackMontage{};

	UPROPERTY(Transient)
	TObjectPtr<UNetherCrownEnemyProjectileDataAsset> CachedProjectileData{};

	FTimerHandle ComboAttackTimerHandle{};
	FTimerHandle ComboAttackEndTimerHandle{};

	UPROPERTY(Replicated)
	int32 CurrentComboIndex{ 0 };

	UPROPERTY(Replicated)
	int32 MaxComboCount{ 0 };

	UPROPERTY(Replicated)
	ENetherCrownEnemyRangedBasicAttackState EnemyRangedBasicAttackState{ ENetherCrownEnemyRangedBasicAttackState::NotAttacking };

	FOnEnemyRangedBasicAttackFinished OnEnemyRangedBasicAttackFinished;
};
