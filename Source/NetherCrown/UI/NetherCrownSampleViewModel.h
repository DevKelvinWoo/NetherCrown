// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "NetherCrownSampleViewModel.generated.h"

UCLASS(BlueprintType, Blueprintable)
class NETHERCROWN_API UNetherCrownSampleViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UNetherCrownSampleViewModel();

	UFUNCTION(BlueprintPure)
	FText GetDisplayName() const
	{
		return DisplayName;
	}

	UFUNCTION(BlueprintCallable)
	void SetDisplayName(const FText& InDisplayName);

private:
	UPROPERTY(BlueprintReadWrite, FieldNotify, BlueprintGetter=GetDisplayName, BlueprintSetter=SetDisplayName, meta=(AllowPrivateAccess="true"))
	FText DisplayName{};
};
