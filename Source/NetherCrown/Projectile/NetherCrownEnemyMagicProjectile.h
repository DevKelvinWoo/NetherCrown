// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetherCrownEnemyMagicProjectile.generated.h"

class UNiagaraComponent;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class NETHERCROWN_API ANetherCrownEnemyMagicProjectile : public AActor
{
	GENERATED_BODY()

public:
	ANetherCrownEnemyMagicProjectile();

	void InitProjectile(const FVector& InDirection, const float ProjectileSpeed);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<USphereComponent> HitSphereComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNiagaraComponent> ProjectileNiagaraComponent{};
};
