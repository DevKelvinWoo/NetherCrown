// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "NetherCrown/Interface/NetherCrownCrowdControlInterface.h"
#include "NetherCrownEnemy.generated.h"

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

	void ApplyCrowdControl(const ENetherCrownCrowdControlType InCrowdControlType, float DurationTime);

	UNetherCrownCrowdControlComponent* GetCrowdControlComponent() const { return CrowdControlComponent; }

	virtual UNetherCrownStatusEffectControlComponent* GetStatusEffectControlComponent() const override;

protected:
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ENetherCrownCrowdControlType GetCrowdControlType() const;

private:
	void ProcessIncomingPhysicalDamage(const AActor* DamageCauser, float DamageAmount);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayTakeDamageSound();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayTakeDamageAnimation(const ENetherCrownCrowdControlType InCrowdControlType);

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCapsuleComponent> EnemyHitBoxComponent{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNetherCrownEnemyStatComponent> EnemyStatComponent{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNetherCrownCrowdControlComponent> CrowdControlComponent{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraComponent> StatusNiagaraComponent{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNetherCrownStatusEffectControlComponent> StatusEffectControlComponent{};

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> TakeDamageAnimMontageSoft{};

	UPROPERTY(EditDefaultsOnly)
	FNetherCrownEnemyTagData EnemyTagData{};
};
