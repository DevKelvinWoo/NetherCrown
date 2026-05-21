// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "NetherCrownQuestComponent.generated.h"

UENUM()
enum class ENetherCrownQuestState : uint8
{
	None,
	InProgress,
	Done
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
	int32 ProgressCount{};
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownQuestComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownQuestComponent();

	void RequestAcceptQuestState(const FGameplayTag& QuestTag, const ENetherCrownQuestState QuestState);
	void RequestGrantQuestReward(const FGameplayTag& QuestTag);

	const ENetherCrownQuestState GetQuestState(const FGameplayTag& QuestTag) const;
	void AddQuestCountProgress(const FGameplayTag& QuestTag, const int32 AddCount);
	int32 GetQuestCountProgress(const FGameplayTag& QuestTag) const;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void SetQuestState(const FGameplayTag& QuestTag, const ENetherCrownQuestState QuestState);

	UFUNCTION(Server, Reliable)
	void Server_SetQuestState(const FGameplayTag& QuestTag, const ENetherCrownQuestState QuestState);

	UFUNCTION(Server, Reliable)
	void Server_GrantQuestReward(const FGameplayTag& QuestTag);

	UPROPERTY(Transient, Replicated)
	TArray<FNetherCrownQuestStateEntry> QuestStateEntries{};

	UPROPERTY(Transient, Replicated)
	TArray<FNetherCrownQuestCountProgressEntry> QuestCountProgressEntries{};
};
