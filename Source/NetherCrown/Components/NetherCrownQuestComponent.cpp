// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownQuestComponent.h"

#include "GameplayTagContainer.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"

UNetherCrownQuestComponent::UNetherCrownQuestComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void UNetherCrownQuestComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UNetherCrownQuestComponent::TickComponent(float DeltaTime, ELevelTick TickType,
FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UNetherCrownQuestComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, QuestStateEntries);
}

void UNetherCrownQuestComponent::SetQuestState(const FGameplayTag& QuestTag, const ENetherCrownQuestState QuestState)
{
	const ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)) || !OwnerCharacter->HasAuthority())
	{
		return;
	}

	if (!QuestTag.IsValid())
	{
		return;
	}

	for (FNetherCrownQuestStateEntry& QuestStateEntry : QuestStateEntries)
	{
		if (QuestStateEntry.QuestTag == QuestTag)
		{
			QuestStateEntry.QuestState = QuestState;
			return;
		}
	}

	FNetherCrownQuestStateEntry QuestStateEntry{};
	QuestStateEntry.QuestTag = QuestTag;
	QuestStateEntry.QuestState = QuestState;
	QuestStateEntries.Add(QuestStateEntry);
}

void UNetherCrownQuestComponent::Server_SetQuestState_Implementation(const FGameplayTag& QuestTag, const ENetherCrownQuestState QuestState)
{
	SetQuestState(QuestTag, QuestState);
}

void UNetherCrownQuestComponent::RequestAcceptQuestState(const FGameplayTag& QuestTag, const ENetherCrownQuestState QuestState)
{
	Server_SetQuestState(QuestTag, QuestState);
}

const ENetherCrownQuestState UNetherCrownQuestComponent::GetQuestState(const FGameplayTag& QuestTag) const
{
	const ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)) || !OwnerCharacter->HasAuthority())
	{
		return ENetherCrownQuestState::None;
	}

	for (const FNetherCrownQuestStateEntry& QuestStateEntry : QuestStateEntries)
	{
		if (QuestStateEntry.QuestTag != QuestTag)
		{
			continue;
		}

		if (QuestStateEntry.QuestState == ENetherCrownQuestState::InProgress)
		{
			return ENetherCrownQuestState::InProgress;
		}
		else if (QuestStateEntry.QuestState == ENetherCrownQuestState::Done)
		{
			return ENetherCrownQuestState::Done;
		}
	}

	return ENetherCrownQuestState::None;
}
