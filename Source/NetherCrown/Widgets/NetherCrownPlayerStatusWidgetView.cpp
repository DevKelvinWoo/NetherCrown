// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownPlayerStatusWidgetView.h"

#include "NetherCrownSkillIconComponent.h"
#include "NetherCrownSliderComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownSkillComponent.h"
#include "ViewModel/NetherCrownPlayerStatusWidgetViewModel.h"

void UNetherCrownPlayerStatusWidgetView::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayerStatusWidgetViewModel = NewObject<UNetherCrownPlayerStatusWidgetViewModel>(this);
	if (!ensureAlways(IsValid(PlayerStatusWidgetViewModel)))
	{
		return;
	}

	PlayerStatusWidgetViewModel->GetOnCharacterMPModified().AddUObject(this, &ThisClass::ApplyMPSlider);
	PlayerStatusWidgetViewModel->GetOnSkillCoolDownModified().AddUObject(this, &ThisClass::ApplySkillCoolDownSlider);

	ANetherCrownCharacter* OwningCharacter{ GetOwningNetherCrownCharacter() };
	if (ensureAlways(IsValid(OwningCharacter)))
	{
		OwningCharacter->GetOnRepPlayerState().AddUObject(this, &ThisClass::HandleOnRepPlayerState);
	}
}

void UNetherCrownPlayerStatusWidgetView::NativeDestruct()
{
	ANetherCrownCharacter* OwningCharacter{ GetOwningNetherCrownCharacter() };
	if (ensureAlways(IsValid(OwningCharacter)))
	{
		OwningCharacter->GetOnRepPlayerState().RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UNetherCrownPlayerStatusWidgetView::HandleOnRepPlayerState()
{
	InitViewModel();
}

void UNetherCrownPlayerStatusWidgetView::ApplyMPSlider(const float RemainMPRatio)
{
	if (!ensureAlways(IsValid(NativeMPBar)))
	{
		return;
	}

	NativeMPBar->SetProgress(RemainMPRatio);
}

void UNetherCrownPlayerStatusWidgetView::ApplySkillCoolDownSlider(const float CoolDownRatio, const ENetherCrownSkillKeyEnum SkillKeyEnum)
{
	if (!ensureAlways(NativeQSkillIcon) || !ensureAlways(NativeESkillIcon) || !ensureAlways(NativeRSkillIcon) || !ensureAlways(NativeShiftSkillIcon))
	{
		return;
	}

	switch (SkillKeyEnum)
	{
	case ENetherCrownSkillKeyEnum::QSkill:
		NativeQSkillIcon->SetSkillCoolDownProgress(CoolDownRatio);
		break;
	case ENetherCrownSkillKeyEnum::ESkill:
		NativeESkillIcon->SetSkillCoolDownProgress(CoolDownRatio);
		break;
	case ENetherCrownSkillKeyEnum::RSkill:
		NativeRSkillIcon->SetSkillCoolDownProgress(CoolDownRatio);
		break;
	case ENetherCrownSkillKeyEnum::ShiftSkill:
		NativeShiftSkillIcon->SetSkillCoolDownProgress(CoolDownRatio);
		break;
	default:
		break;
	}
}

void UNetherCrownPlayerStatusWidgetView::InitViewModel()
{
	if (!ensureAlways(IsValid(PlayerStatusWidgetViewModel)))
	{
		return;
	}

	ANetherCrownCharacter* OwningCharacter{ GetOwningNetherCrownCharacter() };
	if (!ensureAlways(IsValid(OwningCharacter)))
	{
		return;
	}

	PlayerStatusWidgetViewModel->InitWidget(OwningCharacter);
}
