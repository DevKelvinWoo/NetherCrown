// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "NetherCrownNPCDialogueWidgetViewModel.generated.h"

class ANetherCrownCharacter;

UCLASS()
class NETHERCROWN_API UNetherCrownNPCDialogueWidgetViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void InitViewModel(ANetherCrownCharacter* InModelCharacter);
	void ResetViewModel();

	void FinishInteract();

private:
	TWeakObjectPtr<ANetherCrownCharacter> ModelCharacterWeak{};
};
