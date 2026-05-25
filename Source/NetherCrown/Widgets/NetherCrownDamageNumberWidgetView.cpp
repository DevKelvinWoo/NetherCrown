// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownDamageNumberWidgetView.h"

#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"

void UNetherCrownDamageNumberWidgetView::InitDamageNumber(const int32 DamageAmount, const bool bIsCriticalDamage)
{
	if (!ensureAlways(IsValid(DamageNumberText)))
	{
		return;
	}

	DamageNumberText->SetText(FText::AsNumber(DamageAmount));
	DamageNumberText->SetColorAndOpacity(bIsCriticalDamage ? CriticalDamageColor : NormalDamageColor);
}

void UNetherCrownDamageNumberWidgetView::PlayDamageNumberAnimation()
{
	if (!IsValid(DamageNumberAnimation))
	{
		return;
	}

	PlayAnimation(DamageNumberAnimation);
}

void UNetherCrownDamageNumberWidgetView::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(DamageNumberAnimation))
	{
		FWidgetAnimationDynamicEvent AnimationFinishedEvent{};
		AnimationFinishedEvent.BindDynamic(this, &ThisClass::HandleDamageNumberAnimationFinished);
		BindToAnimationFinished(DamageNumberAnimation, AnimationFinishedEvent);
	}
}

void UNetherCrownDamageNumberWidgetView::HandleDamageNumberAnimationFinished()
{
	OnDamageNumberAnimationFinished.Broadcast();
}
