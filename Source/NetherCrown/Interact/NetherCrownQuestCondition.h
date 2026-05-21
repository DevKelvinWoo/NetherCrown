// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "NetherCrownQuestCondition.generated.h"

class ANetherCrownCharacter;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class NETHERCROWN_API UNetherCrownQuestCondition : public UObject
{
	GENERATED_BODY()

public:
	virtual bool IsConditionSatisfied(const ANetherCrownCharacter* QuestOwner, const FGameplayTag& QuestTag) const;
};

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class NETHERCROWN_API UNetherCrownQuestMonsterKillCondition : public UNetherCrownQuestCondition
{
	GENERATED_BODY()

public:
	virtual bool IsConditionSatisfied(const ANetherCrownCharacter* QuestOwner, const FGameplayTag& QuestTag) const override;
};

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class NETHERCROWN_API UNetherCrownQuestFindItemCondition : public UNetherCrownQuestCondition
{
	GENERATED_BODY()

public:
	virtual bool IsConditionSatisfied(const ANetherCrownCharacter* QuestOwner, const FGameplayTag& QuestTag) const override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Condition")
	int32 RequiredWeaponSellQuestItemCount{ 1 };
};
