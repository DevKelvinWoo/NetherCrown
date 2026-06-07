// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "NetherCrownQuestComponent.generated.h"

class UNetherCrownQuestData;

UENUM()
enum class ENetherCrownQuestState : uint8
{
	None,
	InProgress,
	Done,
	Rewarded
};

USTRUCT()
struct FNetherCrownQuestStateEntry
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag QuestTag{};

	UPROPERTY()
	ENetherCrownQuestState QuestState{ ENetherCrownQuestState::None };
};

USTRUCT()
struct FNetherCrownQuestCountProgressEntry
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag QuestTag{};

	UPROPERTY()
	FGameplayTag TargetTag{};

	UPROPERTY()
	int32 ProgressCount{};
};

USTRUCT()
struct FNetherCrownQuestPersistentData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FNetherCrownQuestStateEntry> QuestStateEntries{};

	UPROPERTY()
	TArray<FNetherCrownQuestCountProgressEntry> QuestCountProgressEntries{};
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownQuestComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE(FOnQuestStateUpdated);

	UNetherCrownQuestComponent();

	void RequestAcceptQuestState(const FGameplayTag& QuestTag, const ENetherCrownQuestState QuestState);
	void RequestGrantQuestReward(const FGameplayTag& QuestTag);

	const ENetherCrownQuestState GetQuestState(const FGameplayTag& QuestTag) const;
	void AddQuestCountProgress(const FGameplayTag& QuestTag, const FGameplayTag& TargetTag, const int32 AddCount);
	int32 GetQuestCountProgress(const FGameplayTag& QuestTag, const FGameplayTag& TargetTag) const;
	FNetherCrownQuestPersistentData MakeQuestPersistentData() const;
	bool RestoreQuestFromPersistentData(const FNetherCrownQuestPersistentData& InQuestPersistentData);

	const TArray<FNetherCrownQuestStateEntry>& GetQuestStateEntries() const { return QuestStateEntries; }

	void CheckAndAddQuestCountProgress(const FGameplayTag& TargetTag, const int32 AddCount);
	bool TryCompleteQuest(const FGameplayTag& QuestTag);

	FOnQuestStateUpdated& GetOnQuestStateUpdated() { return OnQuestStateUpdated; }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(const float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UNetherCrownQuestData* GetQuestDataByGameplayTag(const FGameplayTag& QuestTag) const;
	bool CanCompleteQuest(const FGameplayTag& QuestTag) const;
	bool TryGrantQuestReward(const FGameplayTag& QuestTag) const;

	void SetQuestState(const FGameplayTag& QuestTag, const ENetherCrownQuestState QuestState);
	void SetQuestPersistentData() const;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetQuestState(const FGameplayTag& QuestTag, const ENetherCrownQuestState QuestState);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_GrantQuestReward(const FGameplayTag& QuestTag);

	UFUNCTION()
	void OnRep_QuestStateEntries();

	UFUNCTION()
	void OnRep_QuestCountProgressEntries();

	UPROPERTY(Transient, ReplicatedUsing = OnRep_QuestStateEntries)
	TArray<FNetherCrownQuestStateEntry> QuestStateEntries{};

	UPROPERTY(Transient, ReplicatedUsing = OnRep_QuestCountProgressEntries)
	TArray<FNetherCrownQuestCountProgressEntry> QuestCountProgressEntries{};

	FOnQuestStateUpdated OnQuestStateUpdated{};
};
