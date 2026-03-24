// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownCrowdControlTypes.generated.h"

UENUM()
enum class ENetherCrownCrowdControlType : uint8
{
	NONE,
	KNOCK_BACK,
	FROZEN,
	STUN,
};
