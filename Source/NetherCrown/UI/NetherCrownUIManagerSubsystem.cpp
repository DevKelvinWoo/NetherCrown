// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownUIManagerSubsystem.h"

#include "NetherCrownPrimaryLayout.h"
#include "NetherCrownUIScreenBase.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "NetherCrown/NetherCrown.h"

void UNetherCrownUIManagerSubsystem::SetPrimaryLayoutClass(TSubclassOf<UNetherCrownPrimaryLayout> InPrimaryLayoutClass)
{
	PrimaryLayoutClass = InPrimaryLayoutClass;
}

void UNetherCrownUIManagerSubsystem::RegisterScreenDefinition(const FNetherCrownUIScreenDefinition& ScreenDefinition)
{
	if (!ScreenDefinition.ScreenTag.IsValid() || !ScreenDefinition.ScreenClass)
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("Invalid ScreenDefinition in %hs"), __FUNCTION__);
		return;
	}

	ScreenDefinitionMap.Add(ScreenDefinition.ScreenTag, ScreenDefinition);

	if (ScreenDefinition.bShowOnInitialize && IsValid(PrimaryLayoutWidget))
	{
		ShowScreenByTag(ScreenDefinition.ScreenTag);
	}
}

void UNetherCrownUIManagerSubsystem::RegisterScreenDefinitions(const TArray<FNetherCrownUIScreenDefinition>& InScreenDefinitions)
{
	for (const FNetherCrownUIScreenDefinition& ScreenDefinition : InScreenDefinitions)
	{
		RegisterScreenDefinition(ScreenDefinition);
	}
}

UNetherCrownPrimaryLayout* UNetherCrownUIManagerSubsystem::InitializePrimaryLayout()
{
	if (IsValid(PrimaryLayoutWidget))
	{
		return PrimaryLayoutWidget;
	}

	ULocalPlayer* LocalPlayer{ GetLocalPlayer() };
	if (!ensureAlways(IsValid(LocalPlayer)) || !PrimaryLayoutClass)
	{
		return nullptr;
	}

	APlayerController* PlayerController{ LocalPlayer->GetPlayerController(GetWorld()) };
	if (!ensureAlways(IsValid(PlayerController)))
	{
		return nullptr;
	}

	PrimaryLayoutWidget = CreateWidget<UNetherCrownPrimaryLayout>(PlayerController, PrimaryLayoutClass);
	if (!ensureAlways(IsValid(PrimaryLayoutWidget)))
	{
		return nullptr;
	}

	PrimaryLayoutWidget->AddToPlayerScreen();

	for (const TPair<FGameplayTag, FNetherCrownUIScreenDefinition>& ScreenDefinitionPair : ScreenDefinitionMap)
	{
		if (ScreenDefinitionPair.Value.bShowOnInitialize)
		{
			ShowScreenByTag(ScreenDefinitionPair.Key);
		}
	}

	ApplyGameOnlyInputMode();
	return PrimaryLayoutWidget;
}

UNetherCrownUIScreenBase* UNetherCrownUIManagerSubsystem::ShowScreenByTag(const FGameplayTag& ScreenTag)
{
	if (!ScreenTag.IsValid())
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("Invalid ScreenTag in %hs"), __FUNCTION__);
		return nullptr;
	}

	if (TObjectPtr<UNetherCrownUIScreenBase>* ActiveScreenPtr = ActiveScreenMap.Find(ScreenTag))
	{
		ApplyInputModeFromTopScreen();
		return *ActiveScreenPtr;
	}

	UNetherCrownPrimaryLayout* LayoutWidget{ InitializePrimaryLayout() };
	if (!ensureAlways(IsValid(LayoutWidget)))
	{
		return nullptr;
	}

	const FNetherCrownUIScreenDefinition* ScreenDefinitionPtr{ ScreenDefinitionMap.Find(ScreenTag) };
	if (!ensureAlways(ScreenDefinitionPtr) || !ScreenDefinitionPtr->ScreenClass)
	{
		return nullptr;
	}

	APlayerController* PlayerController{ GetLocalPlayer() ? GetLocalPlayer()->GetPlayerController(GetWorld()) : nullptr };
	if (!ensureAlways(IsValid(PlayerController)))
	{
		return nullptr;
	}

	UNetherCrownUIScreenBase* ScreenWidget{ CreateWidget<UNetherCrownUIScreenBase>(PlayerController, ScreenDefinitionPtr->ScreenClass) };
	if (!ensureAlways(IsValid(ScreenWidget)))
	{
		return nullptr;
	}

	LayoutWidget->AddScreenWidget(ScreenWidget, ScreenDefinitionPtr->LayerTag);
	ActiveScreenMap.Add(ScreenTag, ScreenWidget);
	AddScreenToLayerContainer(ScreenWidget, ScreenDefinitionPtr->LayerTag);
	ApplyInputModeFromTopScreen();

	return ScreenWidget;
}

void UNetherCrownUIManagerSubsystem::HideScreenByTag(const FGameplayTag& ScreenTag)
{
	const FNetherCrownUIScreenDefinition* ScreenDefinitionPtr{ ScreenDefinitionMap.Find(ScreenTag) };
	TObjectPtr<UNetherCrownUIScreenBase>* ScreenWidgetPtr{ ActiveScreenMap.Find(ScreenTag) };
	if (!ScreenDefinitionPtr || !ScreenWidgetPtr || !IsValid(*ScreenWidgetPtr))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("Screen is not valid"));
		return;
	}

	if (IsValid(PrimaryLayoutWidget))
	{
		PrimaryLayoutWidget->RemoveScreenWidget(*ScreenWidgetPtr);
	}

	RemoveScreenFromLayerContainer(*ScreenWidgetPtr, ScreenDefinitionPtr->LayerTag);
	ActiveScreenMap.Remove(ScreenTag);
	ApplyInputModeFromTopScreen();
}

void UNetherCrownUIManagerSubsystem::ToggleScreenByTag(const FGameplayTag& ScreenTag)
{
	if (IsScreenActive(ScreenTag))
	{
		HideScreenByTag(ScreenTag);
		return;
	}

	ShowScreenByTag(ScreenTag);
}

bool UNetherCrownUIManagerSubsystem::IsScreenActive(const FGameplayTag& ScreenTag) const
{
	const TObjectPtr<UNetherCrownUIScreenBase>* ScreenWidgetPtr{ ActiveScreenMap.Find(ScreenTag) };
	return ScreenWidgetPtr && IsValid(*ScreenWidgetPtr);
}

UNetherCrownUIScreenBase* UNetherCrownUIManagerSubsystem::GetScreenByTag(const FGameplayTag& ScreenTag) const
{
	const TObjectPtr<UNetherCrownUIScreenBase>* ScreenWidgetPtr{ ActiveScreenMap.Find(ScreenTag) };
	return ScreenWidgetPtr ? *ScreenWidgetPtr : nullptr;
}

void UNetherCrownUIManagerSubsystem::ClearLayer(const FGameplayTag& LayerTag)
{
	TArray<TObjectPtr<UNetherCrownUIScreenBase>> LayerScreens{ GetLayerContainer(LayerTag) };

	for (UNetherCrownUIScreenBase* LayerScreen : LayerScreens)
	{
		if (!IsValid(LayerScreen))
		{
			continue;
		}

		FGameplayTag FoundScreenTag{};

		for (const TPair<FGameplayTag, TObjectPtr<UNetherCrownUIScreenBase>>& ActiveScreenPair : ActiveScreenMap)
		{
			if (ActiveScreenPair.Value == LayerScreen)
			{
				FoundScreenTag = ActiveScreenPair.Key;
				break;
			}
		}

		if (FoundScreenTag.IsValid())
		{
			HideScreenByTag(FoundScreenTag);
		}
	}
}

void UNetherCrownUIManagerSubsystem::Deinitialize()
{
	GameLayerScreenStack.Empty();
	GameMenuLayerScreenStack.Empty();
	MenuLayerScreenStack.Empty();
	ModalLayerScreenStack.Empty();
	ActiveScreenMap.Empty();
	ScreenDefinitionMap.Empty();

	if (IsValid(PrimaryLayoutWidget))
	{
		PrimaryLayoutWidget->RemoveFromParent();
		PrimaryLayoutWidget = nullptr;
	}

	Super::Deinitialize();
}

void UNetherCrownUIManagerSubsystem::AddScreenToLayerContainer(UNetherCrownUIScreenBase* ScreenWidget, const FGameplayTag& LayerTag)
{
	TArray<TObjectPtr<UNetherCrownUIScreenBase>>& LayerContainer{ GetLayerContainer(LayerTag) };
	LayerContainer.Add(ScreenWidget);
}

void UNetherCrownUIManagerSubsystem::RemoveScreenFromLayerContainer(UNetherCrownUIScreenBase* ScreenWidget, const FGameplayTag& LayerTag)
{
	TArray<TObjectPtr<UNetherCrownUIScreenBase>>& LayerContainer{ GetLayerContainer(LayerTag) };
	LayerContainer.Remove(ScreenWidget);
}

TArray<TObjectPtr<UNetherCrownUIScreenBase>>& UNetherCrownUIManagerSubsystem::GetLayerContainer(const FGameplayTag& LayerTag)
{
	if (LayerTag == NetherCrownTags::UI_Layer_Game)
	{
		return GameLayerScreenStack;
	}

	if (LayerTag == NetherCrownTags::UI_Layer_GameMenu)
	{
		return GameMenuLayerScreenStack;
	}

	if (LayerTag == NetherCrownTags::UI_Layer_Menu)
	{
		return MenuLayerScreenStack;
	}

	if (LayerTag == NetherCrownTags::UI_Layer_Modal)
	{
		return ModalLayerScreenStack;
	}

	return GameLayerScreenStack;
}

const TArray<TObjectPtr<UNetherCrownUIScreenBase>>& UNetherCrownUIManagerSubsystem::GetLayerContainer(const FGameplayTag& LayerTag) const
{
	if (LayerTag == NetherCrownTags::UI_Layer_Game)
	{
		return GameLayerScreenStack;
	}

	if (LayerTag == NetherCrownTags::UI_Layer_GameMenu)
	{
		return GameMenuLayerScreenStack;
	}

	if (LayerTag == NetherCrownTags::UI_Layer_Menu)
	{
		return MenuLayerScreenStack;
	}

	if (LayerTag == NetherCrownTags::UI_Layer_Modal)
	{
		return ModalLayerScreenStack;
	}

	return GameLayerScreenStack;
}

UNetherCrownUIScreenBase* UNetherCrownUIManagerSubsystem::GetTopPriorityScreen() const
{
	const TArray<FGameplayTag> LayerPriority
	{
		NetherCrownTags::UI_Layer_Modal,
		NetherCrownTags::UI_Layer_Menu,
		NetherCrownTags::UI_Layer_GameMenu,
		NetherCrownTags::UI_Layer_Game
	};

	for (const FGameplayTag& LayerTag : LayerPriority)
	{
		const TArray<TObjectPtr<UNetherCrownUIScreenBase>>& LayerContainer{ GetLayerContainer(LayerTag) };

		for (int32 Index{ LayerContainer.Num() - 1 }; Index >= 0; --Index)
		{
			if (IsValid(LayerContainer[Index]))
			{
				return LayerContainer[Index];
			}
		}
	}

	return nullptr;
}

void UNetherCrownUIManagerSubsystem::ApplyInputModeFromTopScreen()
{
	APlayerController* PlayerController{ GetLocalPlayer() ? GetLocalPlayer()->GetPlayerController(GetWorld()) : nullptr };
	if (!ensureAlways(IsValid(PlayerController)))
	{
		return;
	}

	UNetherCrownUIScreenBase* TopScreen{ GetTopPriorityScreen() };
	if (!IsValid(TopScreen))
	{
		ApplyGameOnlyInputMode();
		return;
	}

	PlayerController->bShowMouseCursor = TopScreen->ShouldShowMouseCursor();

	switch (TopScreen->GetInputMode())
	{
	case ENetherCrownUIInputMode::GameOnly:
	{
		FInputModeGameOnly InputMode{};
		PlayerController->SetInputMode(InputMode);
		break;
	}
	case ENetherCrownUIInputMode::GameAndUI:
	{
		FInputModeGameAndUI InputMode{};
		InputMode.SetHideCursorDuringCapture(false);
		PlayerController->SetInputMode(InputMode);
		break;
	}
	case ENetherCrownUIInputMode::UIOnly:
	{
		FInputModeUIOnly InputMode{};
		PlayerController->SetInputMode(InputMode);
		break;
	}
	default:
		ApplyGameOnlyInputMode();
		break;
	}
}

void UNetherCrownUIManagerSubsystem::ApplyGameOnlyInputMode() const
{
	APlayerController* PlayerController{ GetLocalPlayer() ? GetLocalPlayer()->GetPlayerController(GetWorld()) : nullptr };
	if (!ensureAlways(IsValid(PlayerController)))
	{
		return;
	}

	FInputModeGameOnly InputMode{};
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = false;
}
