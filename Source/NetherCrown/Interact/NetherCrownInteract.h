// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NetherCrownInteract.generated.h"

class ANetherCrownCharacter;

UINTERFACE()
class UNetherCrownInteract : public UInterface
{
	GENERATED_BODY()
};

class NETHERCROWN_API INetherCrownInteract
{
	GENERATED_BODY()

public:
	virtual void Interact() = 0;
	virtual bool IsNeedCameraMoving() { return false; }
	virtual void FinishInteract(ANetherCrownCharacter* InteractCharacter) {}
};
