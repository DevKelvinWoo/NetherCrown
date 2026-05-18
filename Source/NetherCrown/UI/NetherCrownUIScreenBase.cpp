// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownUIScreenBase.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Character/NetherCrownPlayerController.h"
#include "NetherCrown/PlayerState/NetherCrownPlayerState.h"

void UNetherCrownUIScreenBase::HandleOwningCharacterChanged(ANetherCrownCharacter* NewOwningCharacter)
{
}

void UNetherCrownUIScreenBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BindOwningPlayerController();
}

void UNetherCrownUIScreenBase::NativeConstruct()
{
	Super::NativeConstruct();

	HandleScreenShown();
}

void UNetherCrownUIScreenBase::NativeDestruct()
{
	UnbindOwningPlayerController();
	HandleScreenHidden();

	Super::NativeDestruct();
}

ANetherCrownPlayerController* UNetherCrownUIScreenBase::GetOwningNetherCrownPlayerController() const
{
	return Cast<ANetherCrownPlayerController>(GetOwningPlayer());
}

ANetherCrownCharacter* UNetherCrownUIScreenBase::GetOwningNetherCrownCharacter() const
{
	if (ANetherCrownCharacter* CachedOwningCharacter{ CachedOwningCharacterWeak.Get() })
	{
		return CachedOwningCharacter;
	}

	return Cast<ANetherCrownCharacter>(GetOwningPlayerPawn());
}

void UNetherCrownUIScreenBase::BindOwningPlayerController()
{
	ANetherCrownPlayerController* OwningPlayerController{ GetOwningNetherCrownPlayerController() };
	if (!ensureAlways(IsValid(OwningPlayerController)))
	{
		return;
	}

	OwningPlayerController->GetOnControlledCharacterChanged().RemoveAll(this);
	OwningPlayerController->GetOnControlledCharacterChanged().AddUObject(this, &ThisClass::HandleControlledCharacterChanged);
	HandleControlledCharacterChanged(OwningPlayerController->GetCachedCharacter());
}

void UNetherCrownUIScreenBase::UnbindOwningPlayerController()
{
	if (ANetherCrownPlayerController* OwningPlayerController{ GetOwningNetherCrownPlayerController() })
	{
		OwningPlayerController->GetOnControlledCharacterChanged().RemoveAll(this);
	}

	if (CachedOwningCharacterWeak.IsValid())
	{
		HandleControlledCharacterChanged(nullptr);
	}
}

void UNetherCrownUIScreenBase::HandleControlledCharacterChanged(ANetherCrownCharacter* NewControlledCharacter)
{
	if (CachedOwningCharacterWeak.Get() == NewControlledCharacter)
	{
		return;
	}

	CachedOwningCharacterWeak = MakeWeakObjectPtr(NewControlledCharacter);
	HandleOwningCharacterChanged(NewControlledCharacter);
}
