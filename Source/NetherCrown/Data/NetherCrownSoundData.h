// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NetherCrownSoundData.generated.h"

class USoundCue;

USTRUCT(BlueprintType)
struct FNetherCrownSoundData : public FTableRowBase
{
	GENERATED_BODY()

public:
	const FGameplayTag& GetSoundTag() const { return SoundTag; }
	TSoftObjectPtr<USoundCue> GetSoundCue() const { return SoundCue; }

private:
	UPROPERTY(EditAnywhere)
	FGameplayTag SoundTag;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USoundCue> SoundCue;
};
