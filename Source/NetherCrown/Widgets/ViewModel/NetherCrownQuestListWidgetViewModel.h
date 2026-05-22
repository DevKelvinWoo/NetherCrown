// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "NetherCrown/Components/NetherCrownQuestComponent.h"
#include "NetherCrownQuestListWidgetViewModel.generated.h"

class ANetherCrownCharacter;

UCLASS()
class NETHERCROWN_API UNetherCrownQuestListWidgetViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void InitViewModel(ANetherCrownCharacter* InModelCharacter);

	const TArray<FNetherCrownQuestStateEntry>& GetCurrentQuestStateEntries() const;
	TArray<FText> GetCurrentQuestEntryDescriptions() const;

private:
	TWeakObjectPtr<ANetherCrownCharacter> ModelCharacterWeak{};
};
