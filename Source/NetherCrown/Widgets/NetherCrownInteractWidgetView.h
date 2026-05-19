// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrown/UI/NetherCrownUIScreenBase.h"
#include "NetherCrownInteractWidgetView.generated.h"

class UBorder;

UCLASS()
class NETHERCROWN_API UNetherCrownInteractWidgetView : public UNetherCrownUIScreenBase
{
	GENERATED_BODY()

public:
	void SetInteractWidgetBorderTexture(UTexture2D* InTexture);

private:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UBorder> InteractWidgetBorder{};
};
