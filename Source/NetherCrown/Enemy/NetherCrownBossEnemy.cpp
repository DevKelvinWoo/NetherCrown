// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownBossEnemy.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "NetherCrown/Components/NetherCrownEnemyRangedBasicAttackComponent.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/UI/NetherCrownUIManagerSubsystem.h"

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

void ANetherCrownBossEnemy::SetIsDead(const bool bInIsDead)
{
	Super::SetIsDead(bInIsDead);

	if (!HasAuthority() || !bInIsDead)
	{
		return;
	}

	OnBossEnemyDead.Broadcast();
	Multicast_HideBossEnemyHpWidget();
}

void ANetherCrownBossEnemy::Multicast_HideBossEnemyHpWidget_Implementation()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	const APlayerController* PlayerController{ World->GetFirstPlayerController() };
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

	if (UIManagerSubsystem->IsScreenActive(NetherCrownTags::UI_Screen_BossEnemyHP))
	{
		UIManagerSubsystem->HideScreenByTag(NetherCrownTags::UI_Screen_BossEnemyHP);
	}
}
