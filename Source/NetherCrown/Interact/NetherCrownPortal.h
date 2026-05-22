// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NetherCrownInteractActor.h"
#include "GameFramework/Actor.h"
#include "NetherCrownPortal.generated.h"

UCLASS(Blueprintable)
class NETHERCROWN_API ANetherCrownPortal : public ANetherCrownInteractActor
{
	GENERATED_BODY()

public:
	ANetherCrownPortal();

protected:
	virtual void BeginPlay() override;

	virtual void FinishInteract(ANetherCrownCharacter* InteractCharacter) override;

private:
	void TravelByLevelTag();
};
