// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NetherCrownCrowdControlInterface.generated.h"

class UNetherCrownStatusEffectControlComponent;

UINTERFACE()
class UNetherCrownCrowdControlInterface : public UInterface
{
	GENERATED_BODY()
};

class NETHERCROWN_API INetherCrownCrowdControlInterface
{
	GENERATED_BODY()

public:
	virtual UNetherCrownStatusEffectControlComponent* GetStatusEffectControlComponent() const = 0;
};
