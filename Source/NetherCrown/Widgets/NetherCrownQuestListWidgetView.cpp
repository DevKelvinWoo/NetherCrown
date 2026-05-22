// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownQuestListWidgetView.h"

#include "Components/ListView.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownQuestComponent.h"
#include "NetherCrown/Data/NetherCrownQuestData.h"
#include "NetherCrown/WidgetObject/NetherCrownQuestEntryObject.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"
#include "ViewModel/NetherCrownQuestListWidgetViewModel.h"

void UNetherCrownQuestListWidgetView::NativeOnInitialized()
{
	QuestListWidgetViewModel = NewObject<UNetherCrownQuestListWidgetViewModel>();

	Super::NativeOnInitialized();
}

void UNetherCrownQuestListWidgetView::NativeConstruct()
{
	Super::NativeConstruct();

	ANetherCrownCharacter* OwningCharacter{ GetOwningNetherCrownCharacter() };
	QuestListWidgetViewModel->InitViewModel(OwningCharacter);
	BindQuestComponent(OwningCharacter);
	UpdateQuestStateSnapshot(false);

	RefreshQuestEntry();
}

void UNetherCrownQuestListWidgetView::NativeDestruct()
{
	UnbindQuestComponent();

	Super::NativeDestruct();
}

void UNetherCrownQuestListWidgetView::HandleOwningCharacterChanged(ANetherCrownCharacter* NewOwningCharacter)
{
	Super::HandleOwningCharacterChanged(NewOwningCharacter);

	if (IsValid(QuestListWidgetViewModel))
	{
		QuestListWidgetViewModel->InitViewModel(NewOwningCharacter);
	}

	BindQuestComponent(NewOwningCharacter);
	UpdateQuestStateSnapshot(false);
	RefreshQuestEntry();
}

void UNetherCrownQuestListWidgetView::BindQuestComponent(ANetherCrownCharacter* InOwningCharacter)
{
	UnbindQuestComponent();

	if (!IsValid(InOwningCharacter))
	{
		return;
	}

	UNetherCrownQuestComponent* QuestComponent{ InOwningCharacter->GetQuestComponent() };
	if (!ensureAlways(IsValid(QuestComponent)))
	{
		return;
	}

	QuestComponent->GetOnQuestStateUpdated().AddUObject(this, &ThisClass::HandleQuestStateUpdated);
	BoundQuestComponentWeak = MakeWeakObjectPtr(QuestComponent);
}

void UNetherCrownQuestListWidgetView::UnbindQuestComponent()
{
	UNetherCrownQuestComponent* BoundQuestComponent{ BoundQuestComponentWeak.Get() };
	if (IsValid(BoundQuestComponent))
	{
		BoundQuestComponent->GetOnQuestStateUpdated().RemoveAll(this);
	}

	BoundQuestComponentWeak.Reset();
}

void UNetherCrownQuestListWidgetView::HandleQuestStateUpdated()
{
	UpdateQuestStateSnapshot(true);
	RefreshQuestEntry();
}

void UNetherCrownQuestListWidgetView::UpdateQuestStateSnapshot(const bool bShouldDetectCompletedQuests)
{
	if (!IsValid(QuestListWidgetViewModel))
	{
		return;
	}

	const TArray<FNetherCrownQuestStateEntry>& QuestStateEntries{ QuestListWidgetViewModel->GetCurrentQuestStateEntries() };
	for (const FNetherCrownQuestStateEntry& QuestStateEntry : QuestStateEntries)
	{
		if (!QuestStateEntry.QuestTag.IsValid())
		{
			continue;
		}

		if (QuestStateEntry.QuestState == ENetherCrownQuestState::Done)
		{
			PendingCompleteAnimationQuestTags.Add(QuestStateEntry.QuestTag);
		}
	}
}

void UNetherCrownQuestListWidgetView::RefreshQuestEntry()
{
	if (!ensureAlways(IsValid(QuestListView)) || !ensureAlways(IsValid(QuestListWidgetViewModel)))
	{
		return;
	}

	QuestListView->ClearListItems();

	const TArray<FNetherCrownQuestStateEntry>& QuestStateEntries{ QuestListWidgetViewModel->GetCurrentQuestStateEntries() };
	if (QuestStateEntries.IsEmpty())
	{
		return;
	}

	for (const FNetherCrownQuestStateEntry& QuestStateEntry : QuestStateEntries)
	{
		if (!QuestStateEntry.QuestTag.IsValid() || QuestStateEntry.QuestState == ENetherCrownQuestState::Rewarded)
		{
			continue;
		}

		const UNetherCrownQuestData* QuestData{ FNetherCrownUtilManager::GetQuestDataAssetByGameplayTag(QuestStateEntry.QuestTag) };
		if (!ensureAlways(IsValid(QuestData)))
		{
			continue;
		}

		UNetherCrownQuestEntryObject* QuestEntryObject{ NewObject<UNetherCrownQuestEntryObject>(this) };
		if (!ensureAlways(IsValid(QuestEntryObject)))
		{
			continue;
		}

		QuestEntryObject->SetQuestDescriptionText(QuestData->GetQuestDescription());
		QuestEntryObject->SetShouldPlayCompleteAnimation(PendingCompleteAnimationQuestTags.Contains(QuestStateEntry.QuestTag));

		QuestListView->AddItem(QuestEntryObject);
	}

	PendingCompleteAnimationQuestTags.Empty();
}
