// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownWeaponSellQuestItem.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownQuestComponent.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"

ANetherCrownWeaponSellQuestItem::ANetherCrownWeaponSellQuestItem()
{
	PrimaryActorTick.bCanEverTick = false;

	ItemMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	ItemMeshComponent->SetupAttachment(RootComponent);
}

void ANetherCrownWeaponSellQuestItem::BeginPlay()
{
	Super::BeginPlay();
}

void ANetherCrownWeaponSellQuestItem::Interact()
{
	Super::Interact();

	if (!HasAuthority())
	{
		return;
	}

	ANetherCrownCharacter* InteractCharacter{ InteractTargetCharacterWeak.Get() };
	if (!IsValid(InteractCharacter))
	{
		return;
	}

	UNetherCrownQuestComponent* QuestComponent{ InteractCharacter->GetQuestComponent() };
	if (!ensureAlways(IsValid(QuestComponent)))
	{
		return;
	}

	QuestComponent->AddQuestCountProgress(NetherCrownTags::Quest_WeaponSell, GetInteractActorTag(), 1);
	QuestComponent->TryCompleteQuest(NetherCrownTags::Quest_WeaponSell);
}
