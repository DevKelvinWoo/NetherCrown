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
class ANetherCrownCharacter;

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
	void RequestEnemyAttackByDA(const UNetherCrownEnemyBasicAttackDataAsset* InEnemyBasicAttackDataAsset);

	void SetHandledEnemyWeapon(ANetherCrownEnemyWeapon* InWeapon);

	bool TryGetKnockBackData(float& OutKnockBackDistance, float& OutKnockBackDuration) const;

	bool IsAttacking() const { return EnemyBasicAttackState == ENetherCrownEnemyBasicAttackState::Attacking; }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	int32 GetEnemyAttackDamage() const;

	void StartEnemyAttack(const FNetherCrownEnemyBasicAttackData& InEnemyBasicAttackData, UAnimMontage* InAttackMontage, const bool bNeedAttackAnimMontage = true);
	void SetupBasicAttackTimer(const FNetherCrownEnemyBasicAttackComboData& EnemyBasicAttackComboData);

	void EnableHitTrace();
	void DisableHitTrace();
	void EndAttack();

	void DetectHit();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_InitHitTraceState(const FVector& InLastEndLocation);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayBasicAttackMontage(const FName& ComboMontageSectionName);

	void LoadEnemyBasicAttackData();
	void CacheBasicAttackAnimMontage();

	FVector GetWeaponTraceSocketLocation() const;

	UPROPERTY(Transient)
	FNetherCrownEnemyBasicAttackData EnemyBasicAttackData{};

	UPROPERTY(Transient)
	FNetherCrownEnemyBasicAttackData ActiveEnemyBasicAttackData{};

	UPROPERTY(EditDefaultsOnly, Category = "EnemyBasicAttackDataAsset")
	TSoftObjectPtr<UNetherCrownEnemyBasicAttackDataAsset> EnemyBasicAttackDataAssetSoft{};

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CachedBasicAttackMontage{};

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CachedActiveAttackMontage{};

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownEnemy> CachedOwnerEnemy{};

	UPROPERTY(Replicated)
	bool bEnableHitTrace{ false };

	UPROPERTY(Replicated)
	ENetherCrownEnemyBasicAttackState EnemyBasicAttackState{ ENetherCrownEnemyBasicAttackState::NotAttacking };

	UPROPERTY(Replicated)
	FVector LastEndLocation{};

	UPROPERTY(Replicated)
	TArray<TObjectPtr<ANetherCrownCharacter>> HitIgnoreCharacters{}; //TSet은 Replicate X

	TWeakObjectPtr<ANetherCrownEnemyWeapon> HandledEnemyWeaponWeak{};

	FTimerHandle AttackEndTimerHandle{};
	FTimerHandle EnableHitTraceTimerHandle{};
	FTimerHandle DisableHitTraceTimerHandle{};
};
