// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownInteractNPC.h"
#include "NetherCrownWeaponSellNPC.generated.h"

class UNetherCrownWeaponSellNPCDataAsset;

UCLASS()
class NETHERCROWN_API ANetherCrownWeaponSellNPC : public ANetherCrownInteractNPC
{
	GENERATED_BODY()

public:
	ANetherCrownWeaponSellNPC();

protected:
	virtual void BeginPlay() override;

	virtual void Interact() override;

private:
};
