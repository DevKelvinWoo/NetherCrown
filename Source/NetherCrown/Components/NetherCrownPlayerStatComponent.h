// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrown/Data/NetherCrownPlayerStatData.h"
#include "NetherCrownPlayerStatComponent.generated.h"

class ANetherCrownCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownPlayerStatComponent : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterHPModified, const float);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterMPModified, const float);

public:
	UNetherCrownPlayerStatComponent();

	const FNetherCrownPlayerStat& GetPlayerStatData() const { return PlayerStatData; }

	//Set Stat
	void AddPlayerShield(const int32 InShieldValue);
	void ClearPlayerShield();

	void ModifyMP(const float MPDelta);
	void ModifyHp(const float HpDelta);

	void SetParryingState(const bool bActive);
	bool IsParrying() const;

	//Delegate
	FOnCharacterHPModified& GetOnCharacterHPModified() { return OnCharacterHPModified; }
	FOnCharacterMPModified& GetOnCharacterMPModified() { return OnCharacterMPModified; }

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void CacheOwnerCharacter();

	void LoadPlayerStatData();

	UFUNCTION()
	void OnRep_PlayerStatData(const FNetherCrownPlayerStat& OldPlayerStatData);

	UPROPERTY(Transient, ReplicatedUsing=OnRep_PlayerStatData)
	FNetherCrownPlayerStat PlayerStatData{};

	UPROPERTY(EditDefaultsOnly, Category = "StatDataAsset")
	TSoftObjectPtr<UNetherCrownPlayerStatData> PlayerStatDataAssetSoft{};

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownCharacter> CachedOwnerCharacter{};

	//Delegate
	FOnCharacterHPModified OnCharacterHPModified;
	FOnCharacterMPModified OnCharacterMPModified;
};
