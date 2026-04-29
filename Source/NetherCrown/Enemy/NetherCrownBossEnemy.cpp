// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownBossEnemy.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "NetherCrown/Components/NetherCrownEnemyRangedBasicAttackComponent.h"

ANetherCrownBossEnemy::ANetherCrownBossEnemy()
{
	PrimaryActorTick.bCanEverTick = false;

	RangedBasicAttackComponent = CreateDefaultSubobject<UNetherCrownEnemyRangedBasicAttackComponent>(TEXT("RangedBasicAttackComponent"));

	UCharacterMovementComponent* CharacterMovementComponent{ GetCharacterMovement() };
	if (!ensureAlways(IsValid(CharacterMovementComponent)))
	{
		return;
	}

	CharacterMovementComponent->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = true;
}

void ANetherCrownBossEnemy::BeginPlay()
{
	Super::BeginPlay();
}
