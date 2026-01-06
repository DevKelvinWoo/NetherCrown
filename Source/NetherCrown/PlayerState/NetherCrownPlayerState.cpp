// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownPlayerState.h"

#include "NetherCrown/Components/NetherCrownPlayerStatComponent.h"

ANetherCrownPlayerState::ANetherCrownPlayerState()
{
	PrimaryActorTick.bCanEverTick = false;

	NetherCrownPlayerStatComponent = CreateDefaultSubobject<UNetherCrownPlayerStatComponent>(TEXT("CharacterStatComponent"));
}
