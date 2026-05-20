// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NetherCrownQuestCondition.generated.h"

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class NETHERCROWN_API UNetherCrownQuestCondition : public UObject
{
	GENERATED_BODY()
};

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class NETHERCROWN_API UNetherCrownQuestMonsterKillCondition : public UNetherCrownQuestCondition
{
	GENERATED_BODY()

};

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class NETHERCROWN_API UNetherCrownQuestFindItemCondition : public UNetherCrownQuestCondition
{
	GENERATED_BODY()

};
