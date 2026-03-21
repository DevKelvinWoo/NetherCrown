// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "NetherCrown/Interface/NetherCrownCrowdControlInterface.h"
#include "NetherCrownEnemy.generated.h"

class UNetherCrownEnemyBasicAttackComponent;
class UNiagaraComponent;
class UCapsuleComponent;

class ANetherCrownCharacter;
class UNetherCrownStatusEffectControlComponent;
class UNetherCrownEnemyStatComponent;
class UNetherCrownCrowdControlComponent;

USTRUCT()
struct FNetherCrownEnemyTagData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag EnemyHurtGruntSoundTag{};

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag EnemyHurtImpactSoundTag{};
};

UCLASS()
class NETHERCROWN_API ANetherCrownEnemy : public ACharacter, public INetherCrownCrowdControlInterface
{
	GENERATED_BODY()

public:
	ANetherCrownEnemy();

	void PlayTakeDamageSound() const;

	UNetherCrownCrowdControlComponent* GetCrowdControlComponent() const { return CrowdControlComponent; }
	UNetherCrownEnemyStatComponent* GetEnemyStatComponent() const { return EnemyStatComponent; }
	virtual UNetherCrownStatusEffectControlComponent* GetStatusEffectControlComponent() const override;

protected:
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ENetherCrownCrowdControlType GetCrowdControlType() const;

private:
	void SetEnemyMovementComponentValue();

	void ProcessIncomingPhysicalDamage(const AActor* DamageCauser, float DamageAmount);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayTakeDamageSound();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayTakeDamageAnimation(const ENetherCrownCrowdControlType InCrowdControlType);

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

	UPROPERTY(EditDefaultsOnly, Category = "AnimMontage")
	TSoftObjectPtr<UAnimMontage> TakeDamageAnimMontageSoft{};

	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FNetherCrownEnemyTagData EnemyTagData{};
};
