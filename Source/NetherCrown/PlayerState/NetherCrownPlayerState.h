// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NetherCrown/Components/NetherCrownQuestComponent.h"
#include "NetherCrown/Data/NetherCrownWeaponData.h"
#include "NetherCrownPlayerState.generated.h"

class UNetherCrownPlayerStatComponent;

UCLASS()
class NETHERCROWN_API ANetherCrownPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ANetherCrownPlayerState();

	UNetherCrownPlayerStatComponent* GetNetherCrownPlayerStatComponent() const { return NetherCrownPlayerStatComponent; }
	bool IsParrying() const;

	const FGuid& GetPersistentPlayerId() const { return PersistentPlayerId; }
	void SetPersistentPlayerId(const FGuid& InPersistentPlayerId);

	const FNetherCrownWeaponPersistentData& GetWeaponPersistentData() const { return WeaponPersistentData; }
	void SetWeaponPersistentData(const FNetherCrownWeaponPersistentData& InPersistentData);

	const FNetherCrownQuestPersistentData& GetQuestPersistentData() const { return QuestPersistentData; }
	void SetQuestPersistentData(const FNetherCrownQuestPersistentData& InPersistentData);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual void OverrideWith(APlayerState* PlayerState) override;

private:
	UPROPERTY(EditDefaultsOnly, Replicated)
	TObjectPtr<UNetherCrownPlayerStatComponent> NetherCrownPlayerStatComponent{};

	UPROPERTY(Replicated)
	FGuid PersistentPlayerId{};

	UPROPERTY(Transient)
	FNetherCrownWeaponPersistentData WeaponPersistentData{};

	UPROPERTY(Transient)
	FNetherCrownQuestPersistentData QuestPersistentData{};
};
