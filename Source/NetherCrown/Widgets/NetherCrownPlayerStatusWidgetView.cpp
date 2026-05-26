// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownPlayerStatusWidgetView.h"

#include "NetherCrownSkillIconComponent.h"
#include "NetherCrownSliderComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownEquipComponent.h"
#include "NetherCrown/Components/NetherCrownSkillComponent.h"
#include "NetherCrown/Weapon/NetherCrownWeapon.h"
#include "ViewModel/NetherCrownPlayerStatusWidgetViewModel.h"

void UNetherCrownPlayerStatusWidgetView::NativeOnInitialized()
{
	PlayerStatusWidgetViewModel = NewObject<UNetherCrownPlayerStatusWidgetViewModel>(this);
	if (!ensureAlways(IsValid(PlayerStatusWidgetViewModel)))
	{
		return;
	}

	PlayerStatusWidgetViewModel->GetOnCharacterHPModified().AddUObject(this, &ThisClass::ApplyHealthSlider);
	PlayerStatusWidgetViewModel->GetOnCharacterMPModified().AddUObject(this, &ThisClass::ApplyMPSlider);
	PlayerStatusWidgetViewModel->GetOnSkillCoolDownModified().AddUObject(this, &ThisClass::ApplySkillCoolDownSlider);

	SetSkillIconsActive(false, false);

	Super::NativeOnInitialized();
}

void UNetherCrownPlayerStatusWidgetView::NativeDestruct()
{
	UnbindCharacterDelegates();

	if (ensureAlways(IsValid(PlayerStatusWidgetViewModel)))
	{
		PlayerStatusWidgetViewModel->ResetViewModel();
	}

	Super::NativeDestruct();
}

void UNetherCrownPlayerStatusWidgetView::HandleOwningCharacterChanged(ANetherCrownCharacter* NewOwningCharacter)
{
	Super::HandleOwningCharacterChanged(NewOwningCharacter);

	UnbindCharacterDelegates();

	if (!IsValid(NewOwningCharacter))
	{
		return;
	}

	BoundCharacterWeak = MakeWeakObjectPtr(NewOwningCharacter);

	NewOwningCharacter->GetOnRepPlayerState().RemoveAll(this);
	NewOwningCharacter->GetOnRepPlayerState().AddUObject(this, &ThisClass::InitViewModel);

	UNetherCrownSkillComponent* SkillComponent{ NewOwningCharacter->GetSkillComponent() };
	if (ensureAlways(IsValid(SkillComponent)))
	{
		SkillComponent->GetOnSkillObjectLoaded().RemoveAll(this);
		SkillComponent->GetOnSkillObjectLoaded().AddUObject(this, &ThisClass::InitViewModel);
	}

	UNetherCrownEquipComponent* EquipComponent{ NewOwningCharacter->GetEquipComponent() };
	if (ensureAlways(IsValid(EquipComponent)))
	{
		EquipComponent->GetOnEquipWeapon().RemoveAll(this);
		EquipComponent->GetOnEquipWeapon().AddUObject(this, &ThisClass::HandleOnEquipWeapon);
		SetSkillIconsActive(IsValid(EquipComponent->GetEquippedWeapon()), false);
	}

	InitViewModel();
}

void UNetherCrownPlayerStatusWidgetView::UnbindCharacterDelegates()
{
	//SetSkillIconsActive(false, false);

	ANetherCrownCharacter* BoundCharacter{ BoundCharacterWeak.Get() };
	if (IsValid(BoundCharacter))
	{
		BoundCharacter->GetOnRepPlayerState().RemoveAll(this);

		UNetherCrownSkillComponent* SkillComponent{ BoundCharacter->GetSkillComponent() };
		if (IsValid(SkillComponent))
		{
			SkillComponent->GetOnSkillObjectLoaded().RemoveAll(this);
		}

		UNetherCrownEquipComponent* EquipComponent{ BoundCharacter->GetEquipComponent() };
		if (IsValid(EquipComponent))
		{
			EquipComponent->GetOnEquipWeapon().RemoveAll(this);
		}

		if (IsValid(PlayerStatusWidgetViewModel))
		{
			PlayerStatusWidgetViewModel->ResetViewModel();
		}
	}

	BoundCharacterWeak.Reset();
}

void UNetherCrownPlayerStatusWidgetView::ApplyHealthSlider(const float RemainHPRatio)
{
	if (!ensureAlways(IsValid(NativeHealthBar)))
	{
		return;
	}

	NativeHealthBar->SetProgress(RemainHPRatio);
}

void UNetherCrownPlayerStatusWidgetView::ApplyMPSlider(const float RemainMPRatio)
{
	if (!ensureAlways(IsValid(NativeMPBar)))
	{
		return;
	}

	NativeMPBar->SetProgress(RemainMPRatio);
}

void UNetherCrownPlayerStatusWidgetView::HandleOnEquipWeapon(const bool bEquipWeapon)
{
	SetSkillIconsActive(bEquipWeapon, bEquipWeapon);
}

void UNetherCrownPlayerStatusWidgetView::SetSkillIconsActive(const bool bActive, const bool bPlayActivateAnimation)
{
	if (ensureAlways(IsValid(NativeQSkillIcon)))
	{
		NativeQSkillIcon->SetSkillIconActive(bActive, bPlayActivateAnimation);
	}

	if (ensureAlways(IsValid(NativeESkillIcon)))
	{
		NativeESkillIcon->SetSkillIconActive(bActive, bPlayActivateAnimation);
	}

	if (ensureAlways(IsValid(NativeRSkillIcon)))
	{
		NativeRSkillIcon->SetSkillIconActive(bActive, bPlayActivateAnimation);
	}

	if (ensureAlways(IsValid(NativeShiftSkillIcon)))
	{
		NativeShiftSkillIcon->SetSkillIconActive(bActive, bPlayActivateAnimation);
	}
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

	ANetherCrownCharacter* OwningCharacter{ BoundCharacterWeak.Get() };
	if (!ensureAlways(IsValid(OwningCharacter)))
	{
		return;
	}

	PlayerStatusWidgetViewModel->InitViewModel(OwningCharacter);

	if (ensureAlways(IsValid(NativeQSkillIcon)))
	{
		NativeQSkillIcon->SetHandlingSkillObject(PlayerStatusWidgetViewModel->GetSkillObject(ENetherCrownSkillKeyEnum::QSkill));
	}

	if (ensureAlways(IsValid(NativeESkillIcon)))
	{
		NativeESkillIcon->SetHandlingSkillObject(PlayerStatusWidgetViewModel->GetSkillObject(ENetherCrownSkillKeyEnum::ESkill));
	}

	if (ensureAlways(IsValid(NativeRSkillIcon)))
	{
		NativeRSkillIcon->SetHandlingSkillObject(PlayerStatusWidgetViewModel->GetSkillObject(ENetherCrownSkillKeyEnum::RSkill));
	}

	if (ensureAlways(IsValid(NativeShiftSkillIcon)))
	{
		NativeShiftSkillIcon->SetHandlingSkillObject(PlayerStatusWidgetViewModel->GetSkillObject(ENetherCrownSkillKeyEnum::ShiftSkill));
	}
}
