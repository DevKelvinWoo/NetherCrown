// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrownPlayerStatComponent.generated.h"

USTRUCT()
struct FNetherCrownPlayerStatData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int32 CharacterHP{ 100 };

	UPROPERTY(EditAnywhere)
	int32 CharacterMP{ 100 };

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
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownPlayerStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownPlayerStatComponent();

	const FNetherCrownPlayerStatData& GetPlayerStatData() const { return PlayerStatData; }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditDefaultsOnly)
	FNetherCrownPlayerStatData PlayerStatData{};
};
