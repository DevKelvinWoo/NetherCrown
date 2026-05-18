// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetherCrownInteractActor.generated.h"

UCLASS()
class NETHERCROWN_API ANetherCrownInteractActor : public AActor
{
	GENERATED_BODY()

public:
	ANetherCrownInteractActor();

protected:
	virtual void BeginPlay() override;

private:

};
