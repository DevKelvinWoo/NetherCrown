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

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCapsuleComponent> EnemyHitBoxComponent{};
};
