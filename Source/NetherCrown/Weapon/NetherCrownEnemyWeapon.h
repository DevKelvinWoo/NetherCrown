// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetherCrownEnemyWeapon.generated.h"

class USkeletalMeshComponent;

UCLASS()
class NETHERCROWN_API ANetherCrownEnemyWeapon : public AActor
{
	GENERATED_BODY()

public:
	ANetherCrownEnemyWeapon();

	USkeletalMeshComponent* GetEnemyWeaponSkeletalMeshComponent() const { return EnemyWeaponSkeletalMeshComponent; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<USkeletalMeshComponent> EnemyWeaponSkeletalMeshComponent{};
};
