// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "NetherCrownPlayerStatusWidgetViewModel.generated.h"

class ANetherCrownCharacter;

UCLASS()
class NETHERCROWN_API UNetherCrownPlayerStatusWidgetViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnChacterMPModified, const float);

public:
	void InitWidget(ANetherCrownCharacter* InModelCharacter);

	FOnChacterMPModified& GetOnCharacterMPModified() { return OnCharacterMPModified; }

private:
	void HandleOnCharacterMPModified(const float RemainMPRatio);

	UPROPERTY(Transient)
	TWeakObjectPtr<ANetherCrownCharacter> ModelCharacterWeak{};

	FOnChacterMPModified OnCharacterMPModified;
};
