// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyWeapon.h"

ANetherCrownEnemyWeapon::ANetherCrownEnemyWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	EnemyWeaponSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponSkeletalMeshComponent"));
	EnemyWeaponSkeletalMeshComponent->SetupAttachment(RootComponent);
}
