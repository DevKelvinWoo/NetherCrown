// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NetherCrownEnemy.generated.h"

class UNetherCrownEnemyStatComponent;
class UCapsuleComponent;

class ANetherCrownCharacter;

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
	void ProcessIncomingDamage(const ANetherCrownCharacter* DamageCauser, float DamageAmount);

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCapsuleComponent> EnemyHitBoxComponent{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNetherCrownEnemyStatComponent> EnemyStatComponent{};
};
