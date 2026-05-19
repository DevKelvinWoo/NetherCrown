// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownWeaponSellNPC.h"

#include "NetherCrown/Data/NetherCrownNPCData.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"

ANetherCrownWeaponSellNPC::ANetherCrownWeaponSellNPC()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ANetherCrownWeaponSellNPC::BeginPlay()
{
	Super::BeginPlay();

	CacheWeaponSellNPCData();
}

void ANetherCrownWeaponSellNPC::Interact()
{
	Super::Interact();

	if (!ensureAlways(IsValid(CachedWeaponSellNPCDataAsset)))
	{
		return;
	}

	const FNetherCrownWeaponSellNPCData& WeaponSellNPCData{ CachedWeaponSellNPCDataAsset->GetWeaponSellNPCData() };
	UE_LOG(LogTemp, Warning, TEXT("Test!"));
}

void ANetherCrownWeaponSellNPC::CacheWeaponSellNPCData()
{
	if (WeaponSellNPCDataAssetSoft.IsNull())
	{
		return;
	}

	CachedWeaponSellNPCDataAsset = WeaponSellNPCDataAssetSoft.LoadSynchronous();
}
