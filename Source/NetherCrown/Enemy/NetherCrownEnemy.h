// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NetherCrownEnemy.generated.h"

class UCapsuleComponent;

UCLASS()
class NETHERCROWN_API ANetherCrownEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	ANetherCrownEnemy();

	//@NOTE : Test variable -> Need to remove soon...
	int32 TestHP{ 200 };

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCapsuleComponent> EnemyHitBoxComponent{};
};
