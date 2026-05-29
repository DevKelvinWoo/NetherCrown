// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NetherCrownSliderComponent.generated.h"

class UProgressBar;

UCLASS()
class NETHERCROWN_API UNetherCrownSliderComponent : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetProgress(const float Percent);

private:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FLinearColor FillColor{};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UProgressBar> NativeProgressBar{};
};
