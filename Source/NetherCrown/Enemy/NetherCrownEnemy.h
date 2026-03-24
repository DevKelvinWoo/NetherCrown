// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NetherCrown/Interface/NetherCrownCrowdControlInterface.h"
#include "NetherCrownEnemy.generated.h"

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
	virtual UNetherCrownStatusEffectControlComponent* GetStatusEffectControlComponent() const override;

protected:
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ENetherCrownCrowdControlType GetCrowdControlType() const;

private:
	void AttachEnemyWeapon();

	void SetEnemyMovementComponentValue();

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

	UPROPERTY(EditDefaultsOnly, Category = "EnemyWeaponClass")
	TSubclassOf<ANetherCrownEnemyWeapon> EnemyWeaponClass{};

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownEnemyWeapon> EnemyWeapon{};
};
