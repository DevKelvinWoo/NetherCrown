// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownPPTypes.generated.h"

UENUM()
enum class ENetherCrownPPType : uint8
{
	Default,
	Frozen,
	Charging,
	Lightning,
	LastComboAttack,
};
