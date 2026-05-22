// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "NetherCrownQuestListEntryWidgetView.generated.h"

class UTextBlock;
class UWidgetAnimation;

UCLASS()
class NETHERCROWN_API UNetherCrownQuestListEntryWidgetView : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> QuestDescriptionTextBlock{};

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> QuestCompleteAnimation{};
};
