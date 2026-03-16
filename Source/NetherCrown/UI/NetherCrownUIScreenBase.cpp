// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownUIScreenBase.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Character/NetherCrownPlayerController.h"
#include "NetherCrown/PlayerState/NetherCrownPlayerState.h"

void UNetherCrownUIScreenBase::NativeConstruct()
{
	Super::NativeConstruct();

	HandleScreenShown();
}

void UNetherCrownUIScreenBase::NativeDestruct()
{
	HandleScreenHidden();

	Super::NativeDestruct();
}

ANetherCrownPlayerController* UNetherCrownUIScreenBase::GetOwningNetherCrownPlayerController() const
{
	return Cast<ANetherCrownPlayerController>(GetOwningPlayer());
}

ANetherCrownCharacter* UNetherCrownUIScreenBase::GetOwningNetherCrownCharacter() const
{
	return Cast<ANetherCrownCharacter>(GetOwningPlayerPawn());
}

