// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownSampleViewModel.h"

UNetherCrownSampleViewModel::UNetherCrownSampleViewModel()
{
	DisplayName = FText::FromString(TEXT("Sample ViewModel"));
}

void UNetherCrownSampleViewModel::SetDisplayName(const FText& InDisplayName)
{
	UE_MVVM_SET_PROPERTY_VALUE(DisplayName, InDisplayName);
}
