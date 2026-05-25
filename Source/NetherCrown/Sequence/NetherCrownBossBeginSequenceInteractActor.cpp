// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownBossBeginSequenceInteractActor.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Character/NetherCrownPlayerController.h"
#include "NetherCrown/Enemy/NetherCrownBossEnemy.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/UI/NetherCrownUIManagerSubsystem.h"
#include "NetherCrown/Widgets/NetherCrownBossEnemyHpWidgetView.h"

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

	ToggleInteractBoxCollision(false);

	if (!ensureAlways(IsValid(BossEnemy)) || !ensureAlways(IsValid(SequenceDummyActor)))
	{
		return;
	}

	Multicast_ShowBossEnemyHpWidget();

	BossEnemy->SetActorHiddenInGame(false);
	BossEnemy->SetActorEnableCollision(true);
	BossEnemy->SetActorTickEnabled(true);

	const FVector& BossEnemyLocation{ SequenceDummyActor->GetActorLocation() };
	const FTransform& TargetTransform{ FTransform(SequenceDummyActor->GetActorRotation(), FVector(BossEnemyLocation.X, BossEnemyLocation.Y, BossEnemy->GetActorLocation().Z), BossEnemy->GetActorScale()) };
	BossEnemy->SetActorTransform(TargetTransform);

	SequenceDummyActor->Destroy();
}

void ANetherCrownBossBeginSequenceInteractActor::Multicast_ShowBossEnemyHpWidget_Implementation()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	const ANetherCrownCharacter* InteractTargetCharacter{ InteractCharacterWeak.Get() };
	if (!ensureAlways(IsValid(InteractTargetCharacter)))
	{
		return;
	}

	if (!InteractTargetCharacter->IsLocallyControlled())
	{
		return;
	}

	const ANetherCrownPlayerController* PlayerController{ Cast<ANetherCrownPlayerController>(InteractTargetCharacter->GetController()) };
	if (!ensureAlways(IsValid(PlayerController)))
	{
		return;
	}

	const ULocalPlayer* LocalPlayer{ PlayerController->GetLocalPlayer() };
	if (!ensureAlways(IsValid(LocalPlayer)))
	{
		return;
	}

	UNetherCrownUIManagerSubsystem* UIManagerSubsystem{ LocalPlayer->GetSubsystem<UNetherCrownUIManagerSubsystem>() };
	if (!ensureAlways(IsValid(UIManagerSubsystem)))
	{
		return;
	}

	UNetherCrownBossEnemyHpWidgetView* BossEnemyHpWidgetView{ Cast<UNetherCrownBossEnemyHpWidgetView>(UIManagerSubsystem->ShowScreenByTag(NetherCrownTags::UI_Screen_BossEnemyHP)) };
	if (!ensureAlways(IsValid(BossEnemyHpWidgetView)))
	{
		return;
	}

	BossEnemyHpWidgetView->InitNormalEnemyHPWidget(BossEnemy);
}

