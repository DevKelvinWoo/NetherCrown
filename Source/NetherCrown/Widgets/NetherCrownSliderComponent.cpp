// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownSliderComponent.h"

#include "Components/ProgressBar.h"
#include "NetherCrown/NetherCrown.h"

void UNetherCrownSliderComponent::SetProgress(float Percent)
{
	if (!ensureAlways(IsValid(NativeProgressBar)))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("ProgressBar is invalid in %hs"), __FUNCTION__);
		return;
	}

	NativeProgressBar->SetPercent(Percent);
}
