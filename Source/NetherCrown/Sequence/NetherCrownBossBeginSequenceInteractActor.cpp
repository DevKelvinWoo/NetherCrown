// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownBossBeginSequenceInteractActor.h"

#include "NetherCrown/Enemy/NetherCrownBossEnemy.h"

ANetherCrownBossBeginSequenceInteractActor::ANetherCrownBossBeginSequenceInteractActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ANetherCrownBossBeginSequenceInteractActor::BeginPlay()
{
	Super::BeginPlay();
}

void ANetherCrownBossBeginSequenceInteractActor::HandleOnSequenceFinished()
{
	Super::HandleOnSequenceFinished();

	if (!HasAuthority())
	{
		return;
	}

	if (!ensureAlways(IsValid(BossEnemy)) || !ensureAlways(IsValid(SequenceDummyActor)))
	{
		return;
	}

	BossEnemy->SetActorHiddenInGame(false);
	BossEnemy->SetActorEnableCollision(true);
	BossEnemy->SetActorTickEnabled(true);

	const FVector& BossEnemyLocation{ SequenceDummyActor->GetActorLocation() };
	const FTransform& TargetTransform{ FTransform(SequenceDummyActor->GetActorRotation(), FVector(BossEnemyLocation.X, BossEnemyLocation.Y, BossEnemy->GetActorLocation().Z), BossEnemy->GetActorScale()) };
	BossEnemy->SetActorTransform(TargetTransform);

	SequenceDummyActor->Destroy();
}

