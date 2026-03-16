// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownSkillIconComponent.h"

#include "Components/ProgressBar.h"

void UNetherCrownSkillIconComponent::SetSkillCoolDownProgress(float Percent)
{
	if (!ensureAlways(NativeSkillThumbnailProgressBar))
	{
		return;
	}

	NativeSkillThumbnailProgressBar->SetPercent(Percent);
}
