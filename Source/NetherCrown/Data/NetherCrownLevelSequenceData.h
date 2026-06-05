// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NetherCrownLevelSequenceData.generated.h"

class ULevelSequence;

USTRUCT(Blueprintable)
struct FNetherCrownLevelSequenceData : public FTableRowBase
{
	GENERATED_BODY()

public:
	const FGameplayTag& GetLevelSequenceTag() const { return LevelSequenceTag; }
	const TSoftObjectPtr<ULevelSequence>& GetLevelSequenceSoft() const { return LevelSequenceSoft; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "LevelSequenceTag")
	FGameplayTag LevelSequenceTag{};

	UPROPERTY(EditDefaultsOnly, Category = "LevelSequenceData")
	TSoftObjectPtr<ULevelSequence> LevelSequenceSoft{};
};
