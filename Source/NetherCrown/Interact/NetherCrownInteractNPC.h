// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownInteract.h"
#include "GameFramework/Character.h"
#include "NetherCrownInteractNPC.generated.h"

enum class ENetherCrownQuestState : uint8;
class UNetherCrownNPCDataAsset;
class USphereComponent;
class UWidgetComponent;

class ANetherCrownCharacter;

UCLASS()
class NETHERCROWN_API ANetherCrownInteractNPC : public ACharacter, public INetherCrownInteract
{
	GENERATED_BODY()

public:
	ANetherCrownInteractNPC();

protected:
	virtual void BeginPlay() override;

	virtual void Interact() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void SetTargetInteractNPC(const ANetherCrownCharacter* InteractCharacter, bool bTargetValid);
	void SetInteractWidgetVisibility(const ANetherCrownCharacter* InteractTarget, bool bVisible) const;

	void CacheNPCDataAsset();

	const ENetherCrownQuestState GetTargetPlayerQuestState() const;
	TArray<FText> GetQuestDialogueText(const ENetherCrownQuestState InQuestState) const;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowNPCDialogueWidget(const ENetherCrownQuestState QuestState);

	UFUNCTION()
	void HandleOnDetectSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleOnDetectSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<USphereComponent> InteractDetectSphereComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UWidgetComponent> InteractWidgetComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TSoftObjectPtr<UNetherCrownNPCDataAsset> NPCDataAssetSoft{};

	UPROPERTY(Transient)
	TObjectPtr<UNetherCrownNPCDataAsset> CachedNPCDataAsset{};

	UPROPERTY(Transient, Replicated)
	int32 CurrentQuestIndex{};

	UPROPERTY(Replicated)
	TWeakObjectPtr<ANetherCrownCharacter> InteractTargetCharacterWeak{};
};
