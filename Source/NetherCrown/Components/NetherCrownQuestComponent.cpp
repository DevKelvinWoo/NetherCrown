// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownQuestComponent.h"

#include "GameplayTagContainer.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Data/NetherCrownQuestData.h"
#include "NetherCrown/Interact/NetherCrownQuestCondition.h"
#include "NetherCrown/Subsystems/NetherCrownQuestManagerSubsystem.h"

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
	DOREPLIFETIME(ThisClass, QuestCountProgressEntries);
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

void UNetherCrownQuestComponent::RequestGrantQuestReward(const FGameplayTag& QuestTag)
{
	Server_GrantQuestReward(QuestTag);
}

void UNetherCrownQuestComponent::Server_GrantQuestReward_Implementation(const FGameplayTag& QuestTag)
{
	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)) || !OwnerCharacter->HasAuthority())
	{
		return;
	}

	if (!QuestTag.IsValid() || GetQuestState(QuestTag) != ENetherCrownQuestState::Done)
	{
		return;
	}

	const UGameInstance* GameInstance{ OwnerCharacter->GetGameInstance() };
	if (!ensureAlways(IsValid(GameInstance)))
	{
		return;
	}

	const UNetherCrownQuestManagerSubsystem* QuestManagerSubsystem{ GameInstance->GetSubsystem<UNetherCrownQuestManagerSubsystem>() };
	if (!ensureAlways(IsValid(QuestManagerSubsystem)))
	{
		return;
	}

	QuestManagerSubsystem->TryGrantQuestReward(OwnerCharacter, QuestTag);
}

const ENetherCrownQuestState UNetherCrownQuestComponent::GetQuestState(const FGameplayTag& QuestTag) const
{
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

void UNetherCrownQuestComponent::AddQuestCountProgress(const FGameplayTag& QuestTag, const FGameplayTag& TargetTag, const int32 AddCount)
{
	const ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)) || !OwnerCharacter->HasAuthority())
	{
		return;
	}

	if (!QuestTag.IsValid() || AddCount <= 0)
	{
		return;
	}

	for (FNetherCrownQuestCountProgressEntry& QuestProgressEntry : QuestCountProgressEntries)
	{
		if (QuestProgressEntry.QuestTag == QuestTag && QuestProgressEntry.TargetTag == TargetTag)
		{
			QuestProgressEntry.ProgressCount += AddCount;
			return;
		}
	}

	FNetherCrownQuestCountProgressEntry QuestProgressEntry{};
	QuestProgressEntry.QuestTag = QuestTag;
	QuestProgressEntry.TargetTag = TargetTag;
	QuestProgressEntry.ProgressCount = AddCount;
	QuestCountProgressEntries.Add(QuestProgressEntry);
}

int32 UNetherCrownQuestComponent::GetQuestCountProgress(const FGameplayTag& QuestTag, const FGameplayTag& TargetTag) const
{
	if (!QuestTag.IsValid())
	{
		return 0;
	}

	for (const FNetherCrownQuestCountProgressEntry& QuestProgressEntry : QuestCountProgressEntries)
	{
		if (QuestProgressEntry.QuestTag == QuestTag && QuestProgressEntry.TargetTag == TargetTag)
		{
			return QuestProgressEntry.ProgressCount;
		}
	}

	return 0;
}

void UNetherCrownQuestComponent::CheckAndAddQuestCountProgress(const FGameplayTag& TargetTag, const int32 AddCount)
{
	if (!TargetTag.IsValid())
	{
		return;
	}

	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)) || !OwnerCharacter->HasAuthority())
	{
		return;
	}

	const UGameInstance* GameInstance{ OwnerCharacter->GetGameInstance() };
	if (!ensureAlways(IsValid(GameInstance)))
	{
		return;
	}

	const UNetherCrownQuestManagerSubsystem* QuestManagerSubsystem{ GameInstance->GetSubsystem<UNetherCrownQuestManagerSubsystem>() };
	if (!ensureAlways(IsValid(QuestManagerSubsystem)))
	{
		return;
	}

	for (FNetherCrownQuestStateEntry& QuestStateEntry : QuestStateEntries)
	{
		if (QuestStateEntry.QuestState != ENetherCrownQuestState::InProgress)
		{
			continue;
		}

		UNetherCrownQuestData* QuestData{ QuestManagerSubsystem->GetQuestDataByGameplayTag(QuestStateEntry.QuestTag) };
		if (!IsValid(QuestData))
		{
			continue;
		}

		const UNetherCrownQuestCondition* QuestCondition{ QuestData->GetQuestCondition() };
		if (!ensureAlways(IsValid(QuestCondition)))
		{
			continue;
		}

		if (QuestCondition->IsConditionTargetEqual(TargetTag))
		{
			AddQuestCountProgress(QuestStateEntry.QuestTag, TargetTag, AddCount);
			QuestManagerSubsystem->TryCompleteQuest(OwnerCharacter, QuestStateEntry.QuestTag);
		}
	}
}
