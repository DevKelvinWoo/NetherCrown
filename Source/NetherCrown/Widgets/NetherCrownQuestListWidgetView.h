// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrown/UI/NetherCrownUIScreenBase.h"
#include "NetherCrown/Components/NetherCrownQuestComponent.h"
#include "NetherCrownQuestListWidgetView.generated.h"

class UNetherCrownQuestListWidgetViewModel;
class UNetherCrownQuestComponent;
class ANetherCrownCharacter;
class UListView;

UCLASS()
class NETHERCROWN_API UNetherCrownQuestListWidgetView : public UNetherCrownUIScreenBase
{
	GENERATED_BODY()

public:

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void HandleOwningCharacterChanged(ANetherCrownCharacter* NewOwningCharacter) override;

private:
	void BindQuestComponent(ANetherCrownCharacter* InOwningCharacter);
	void UnbindQuestComponent();
	void HandleQuestStateUpdated();
	void UpdateQuestStateSnapshot(const bool bShouldDetectCompletedQuests);
	void RefreshQuestEntry();

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UListView> QuestListView{};

	UPROPERTY(Transient)
	TObjectPtr<UNetherCrownQuestListWidgetViewModel> QuestListWidgetViewModel{};

	TWeakObjectPtr<UNetherCrownQuestComponent> BoundQuestComponentWeak{};

	TSet<FGameplayTag> PendingCompleteAnimationQuestTags{};
};
