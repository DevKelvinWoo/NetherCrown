// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NetherCrown/Interface/NetherCrownCrowdControlInterface.h"
#include "NetherCrownEnemy.generated.h"

class UNetherCrownEnemySkillComponent;
class UNetherCrownEnemyBTCosmeticComponent;
class ANetherCrownEnemyWeapon;
class UNiagaraComponent;
class UCapsuleComponent;

class UNetherCrownStatusEffectControlComponent;
class UNetherCrownEnemyStatComponent;
class UNetherCrownCrowdControlComponent;
class UNetherCrownEnemyBasicAttackComponent;
class UNetherCrownEnemyDamageReceiverComponent;

UCLASS()
class NETHERCROWN_API ANetherCrownEnemy : public ACharacter, public INetherCrownCrowdControlInterface
{
	GENERATED_BODY()

public:
	ANetherCrownEnemy();

	UNetherCrownCrowdControlComponent* GetCrowdControlComponent() const { return CrowdControlComponent; }
	UNetherCrownEnemyStatComponent* GetEnemyStatComponent() const { return EnemyStatComponent; }
	UNetherCrownEnemyBTCosmeticComponent* GetEnemyBTCosmeticComponent() const { return EnemyBTCosmeticComponent; }
	UNetherCrownEnemySkillComponent* GetEnemySkillComponent() const { return EnemySkillComponent; }

	virtual UNetherCrownStatusEffectControlComponent* GetStatusEffectControlComponent() const override;

	void SetIsDead(const bool InbIsDead);
	bool IsDead() { return bIsDead; }

protected:
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ENetherCrownCrowdControlType GetCrowdControlType() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsDead() const { return bIsDead; }

private:
	void AttachEnemyWeapon();

	void SetEnemyMovementComponentValue();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetHitBoxCollisionEnabled(const bool bEnable);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetCapsuleCollisionResponse(const ECollisionChannel Channel, const ECollisionResponse Response);

	UFUNCTION(NetMulticast, UnReliable)
	void Multicast_DeActiveStatusNiagaraSystem();

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UCapsuleComponent> EnemyHitBoxComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNetherCrownEnemyStatComponent> EnemyStatComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNetherCrownCrowdControlComponent> CrowdControlComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNiagaraComponent> StatusNiagaraComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNetherCrownStatusEffectControlComponent> StatusEffectControlComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNetherCrownEnemyBasicAttackComponent> BasicAttackComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNetherCrownEnemyDamageReceiverComponent> EnemyDamageReceiverComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNetherCrownEnemyBTCosmeticComponent> EnemyBTCosmeticComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNetherCrownEnemySkillComponent> EnemySkillComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "EnemyWeaponClass")
	TSubclassOf<ANetherCrownEnemyWeapon> EnemyWeaponClass{};

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownEnemyWeapon> EnemyWeapon{};

	UPROPERTY(Transient, Replicated)
	bool bIsDead{ false };
};
