// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NetherCrownPlayerState.generated.h"

class UNetherCrownPlayerStatComponent;

UCLASS()
class NETHERCROWN_API ANetherCrownPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	ANetherCrownPlayerState();

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNetherCrownPlayerStatComponent> NetherCrownPlayerStatComponent{};
};
