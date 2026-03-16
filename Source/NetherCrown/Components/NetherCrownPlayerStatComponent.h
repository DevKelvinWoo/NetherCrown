// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrownPlayerStatComponent.generated.h"

class ANetherCrownCharacter;

USTRUCT()
struct FNetherCrownPlayerStatData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int32 CharacterHP{ 100 };

	UPROPERTY(EditAnywhere)
	float CharacterMaxHP{ 100.f };

	UPROPERTY(EditAnywhere)
	float CharacterMP{ 100.f };

	UPROPERTY(EditAnywhere)
	float CharacterMaxMP{ 100.f };

	UPROPERTY(EditAnywhere)
	int32 CharacterEXP{ 0 };

	UPROPERTY(EditAnywhere)
	int32 CharacterLevel{ 1 };

	UPROPERTY(EditAnywhere)
	int32 CharacterMaxLevel{ 10 };

	UPROPERTY(EditAnywhere)
	int32 CharacterGold{ 0 };

	UPROPERTY(EditAnywhere)
	int32 CharacterMaxGold{ 1000 };

	UPROPERTY(EditAnywhere)
	int32 PhysicalArmor{ 10 };

	UPROPERTY(EditAnywhere)
	int32 MagicArmor{ 10 };

	UPROPERTY(EditAnywhere)
	int32 AttackDamage{ 10 };

	UPROPERTY(EditAnywhere)
	int32 MagicPower{ 10 };

	UPROPERTY(EditAnywhere)
	int32 ShieldValue{ 0 };
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownPlayerStatComponent : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnChacterMPModified, const float);

public:
	UNetherCrownPlayerStatComponent();

	const FNetherCrownPlayerStatData& GetPlayerStatData() const { return PlayerStatData; }

	//Set Stat
	void AddPlayerShield(int32 InShieldValue);
	void ClearPlayerShield();

	void ModifyMP(float MPDelta);

	//Delegate
	FOnChacterMPModified& GetOnCharacterMPModified() { return OnCharacterMPModified; }

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnRep_PlayerStatData(const FNetherCrownPlayerStatData& OldPlayerStatData);

	UPROPERTY(EditDefaultsOnly, ReplicatedUsing=OnRep_PlayerStatData, Category = "StatData")
	FNetherCrownPlayerStatData PlayerStatData{};

	//Delegate
	FOnChacterMPModified OnCharacterMPModified;
};
