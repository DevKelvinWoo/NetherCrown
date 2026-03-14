// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "NetherCrownUITypes.generated.h"

class UNetherCrownUIScreenBase;

UENUM(BlueprintType)
enum class ENetherCrownUIInputMode : uint8
{
	GameOnly,
	GameAndUI,
	UIOnly
};

USTRUCT(BlueprintType)
struct FNetherCrownUIScreenDefinition : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ScreenTag{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag LayerTag{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UNetherCrownUIScreenBase> ScreenClass{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowOnInitialize{ false };
};
