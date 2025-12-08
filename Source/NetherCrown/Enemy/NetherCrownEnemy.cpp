// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemy.h"

#include "Components/CapsuleComponent.h"

ANetherCrownEnemy::ANetherCrownEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	EnemyHitBoxComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("EnemyHitBoxComponent"));
	EnemyHitBoxComponent->SetupAttachment(RootComponent);
}

void ANetherCrownEnemy::BeginPlay()
{
	Super::BeginPlay();

}

void ANetherCrownEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
