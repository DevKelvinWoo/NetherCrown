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

	FVector GetInteractCameraPos() const;
	FRotator GetInteractCameraRot() const;

protected:
	virtual void BeginPlay() override;

	virtual bool IsNeedCameraMoving() const override { return true; };
	virtual void Interact() override;
	virtual void FinishInteract(ANetherCrownCharacter* InteractCharacter) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void SetTargetInteractNPC(const ANetherCrownCharacter* InteractCharacter, const bool bTargetValid);
	void SetInteractWidgetVisibility(const ANetherCrownCharacter* InteractTarget, const bool bVisible) const;

	void CacheNPCDataAsset();

	int32 GetCurrentQuestIndexForPlayer(const ANetherCrownCharacter* InteractCharacter) const;
	const ENetherCrownQuestState GetTargetPlayerQuestState() const;
	TArray<FText> GetQuestDialogueText(const int32 QuestIndex, const ENetherCrownQuestState InQuestState) const;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowNPCDialogueWidget(const int32 QuestIndex, const ENetherCrownQuestState QuestState);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetInteractWidgetVisibility(const ANetherCrownCharacter* InteractTarget, const bool bVisible);

	UFUNCTION()
	void HandleOnDetectSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, const int32 OtherBodyIndex, const bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleOnDetectSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, const int32 OtherBodyIndex);

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<USphereComponent> InteractDetectSphereComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UWidgetComponent> InteractWidgetComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<USphereComponent> InteractCameraPosSphereComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TSoftObjectPtr<UNetherCrownNPCDataAsset> NPCDataAssetSoft{};

	UPROPERTY(Transient)
	TObjectPtr<UNetherCrownNPCDataAsset> CachedNPCDataAsset{};

	UPROPERTY(Replicated)
	TWeakObjectPtr<ANetherCrownCharacter> InteractTargetCharacterWeak{};
};
