// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NetherCrownQuestReward.generated.h"

class ANetherCrownCharacter;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class NETHERCROWN_API UNetherCrownQuestReward : public UObject
{
	GENERATED_BODY()

public:
	virtual bool GrantReward(ANetherCrownCharacter* QuestOwner) const;
};

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class NETHERCROWN_API UNetherCrownQuestWeaponReward : public UNetherCrownQuestReward
{
	GENERATED_BODY()

public:
	virtual bool GrantReward(ANetherCrownCharacter* QuestOwner) const override;
};
