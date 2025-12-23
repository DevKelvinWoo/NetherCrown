// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "NetherCrownEnemy.generated.h"

class UNetherCrownEnemyStatComponent;
class UCapsuleComponent;

class ANetherCrownCharacter;

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
class NETHERCROWN_API ANetherCrownEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	ANetherCrownEnemy();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
	void ProcessIncomingDamage(const AActor* DamageCauser, float DamageAmount);

	void PlayTakeDamageSound();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayTakeDamageSound();

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCapsuleComponent> EnemyHitBoxComponent{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNetherCrownEnemyStatComponent> EnemyStatComponent{};

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> TakeDamageAnimMontageSoft{};

	UPROPERTY(EditDefaultsOnly)
	FNetherCrownEnemyTagData EnemyTagData{};
};
