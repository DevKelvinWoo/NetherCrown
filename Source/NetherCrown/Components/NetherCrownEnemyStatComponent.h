// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrownEnemyStatComponent.generated.h"

USTRUCT()
struct FNetherCrownEnemyStatData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int32 EnemyHP{ 100 };

	UPROPERTY(EditAnywhere)
	int32 AttackDamage{ 10 };

	UPROPERTY(EditAnywhere)
	int32 MagicPower{ 10 };

	UPROPERTY(EditAnywhere)
	int32 PhysicalArmor{ 10 };

	UPROPERTY(EditAnywhere)
	int32 MagicArmor{ 10 };
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownEnemyStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownEnemyStatComponent();

	const FNetherCrownEnemyStatData& GetEnemyStatData() const { return EnemyStatData; }

	void SetEnemyHp (int32 InHp);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditDefaultsOnly)
	FNetherCrownEnemyStatData EnemyStatData{};
};
