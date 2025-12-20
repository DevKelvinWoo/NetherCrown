// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemy.h"

#include "Components/CapsuleComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"

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

float ANetherCrownEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ResultDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(DamageCauser) };
	if (!ensureAlways(IsValid(OwnerCharacter)))
	{
		return 0.f;
	}

	//@TODO : Enemy Stat구현 후 방어구 관통 계산 후 데미지 입히기

	TestHP -= DamageAmount;
	if (TestHP <= 0)
	{
		Destroy();
	}

	return ResultDamage;
}
