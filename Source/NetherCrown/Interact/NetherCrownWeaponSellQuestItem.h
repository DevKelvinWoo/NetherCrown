// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownInteractActor.h"
#include "NetherCrownWeaponSellQuestItem.generated.h"

class UStaticMeshComponent;

UCLASS()
class NETHERCROWN_API ANetherCrownWeaponSellQuestItem : public ANetherCrownInteractActor
{
	GENERATED_BODY()

public:
	ANetherCrownWeaponSellQuestItem();

protected:
	virtual void BeginPlay() override;

	virtual void FinishInteract(ANetherCrownCharacter* InteractCharacter) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UStaticMeshComponent> ItemMeshComponent{};
};
