// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownQuestListEntryWidgetView.h"

#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"
#include "NetherCrown/WidgetObject/NetherCrownQuestEntryObject.h"

void UNetherCrownQuestListEntryWidgetView::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	StopAllAnimations();

	const UNetherCrownQuestEntryObject* QuestEntryObject{ Cast<UNetherCrownQuestEntryObject>(ListItemObject) };
	if (!IsValid(QuestEntryObject))
	{
		return;
	}

	if (!ensureAlways(IsValid(QuestDescriptionTextBlock)))
	{
		return;
	}

	QuestDescriptionTextBlock->SetText(QuestEntryObject->GetQuestDescriptionText());

	if (QuestEntryObject->ShouldPlayCompleteAnimation() && IsValid(QuestComplyeteAnimation.Get()))
	{
		PlayAnimation(QuestCompleteAnimation, 0.f, 0);
	}
}
