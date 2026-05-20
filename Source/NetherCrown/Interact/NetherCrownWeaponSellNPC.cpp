// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownWeaponSellNPC.h"

#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"

ANetherCrownWeaponSellNPC::ANetherCrownWeaponSellNPC()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ANetherCrownWeaponSellNPC::BeginPlay()
{
	Super::BeginPlay();
}

void ANetherCrownWeaponSellNPC::Interact()
{
	Super::Interact();
}
