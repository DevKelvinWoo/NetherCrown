// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownNPCDialogueWidgetViewModel.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownInteractComponent.h"

void UNetherCrownNPCDialogueWidgetViewModel::InitViewModel(ANetherCrownCharacter* InModelCharacter)
{
	if (!ensureAlways(IsValid(InModelCharacter)))
	{
		return;
	}

	ModelCharacterWeak = MakeWeakObjectPtr(InModelCharacter);
}

void UNetherCrownNPCDialogueWidgetViewModel::ResetViewModel()
{
	ModelCharacterWeak.Reset();
}

void UNetherCrownNPCDialogueWidgetViewModel::FinishInteract()
{
	ANetherCrownCharacter* ModelCharacter{ ModelCharacterWeak.Get() };
	if (!ensureAlways(IsValid(ModelCharacter)))
	{
		return;
	}

	UNetherCrownInteractComponent* InteractComponent{ ModelCharacter->GetInteractComponent() };
	if (!ensureAlways(IsValid(InteractComponent)))
	{
		return;
	}

	InteractComponent->FinishInteract();
}
