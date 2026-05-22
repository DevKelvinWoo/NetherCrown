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
	virtual bool IsConditionTargetEqual(const FGameplayTag& TargetTag) const;
};

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class NETHERCROWN_API UNetherCrownQuestMonsterKillCondition : public UNetherCrownQuestCondition
{
	GENERATED_BODY()

public:
	virtual bool IsConditionSatisfied(const ANetherCrownCharacter* QuestOwner, const FGameplayTag& QuestTag) const override;
	virtual bool IsConditionTargetEqual(const FGameplayTag& TargetTag) const override;

	const FGameplayTag& GetRequiredQuestTargetTag() const { return RequiredQuestTargetTag; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Condition")
	int32 RequiredKillCount{ 3 };

	UPROPERTY(EditDefaultsOnly, Category = "Condition")
	FGameplayTag RequiredQuestTargetTag{};
};

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class NETHERCROWN_API UNetherCrownQuestFindItemCondition : public UNetherCrownQuestCondition
{
	GENERATED_BODY()

public:
	virtual bool IsConditionSatisfied(const ANetherCrownCharacter* QuestOwner, const FGameplayTag& QuestTag) const override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Condition")
	int32 RequiredQuestItemCount{ 1 };

	UPROPERTY(EditDefaultsOnly, Category = "Condition")
	FGameplayTag RequiredQuestTargetTag{};
};
