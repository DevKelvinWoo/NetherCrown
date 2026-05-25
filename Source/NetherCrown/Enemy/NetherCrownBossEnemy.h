// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownEnemy.h"
#include "NetherCrownBossEnemy.generated.h"

class UNetherCrownEnemyRangedBasicAttackComponent;

UCLASS()
class NETHERCROWN_API ANetherCrownBossEnemy : public ANetherCrownEnemy
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE(FOnBossEnemyDead);

public:
	ANetherCrownBossEnemy();

	UNetherCrownEnemyRangedBasicAttackComponent* GetRangedBasicAttackComponent() const { return RangedBasicAttackComponent; }

	FOnBossEnemyDead& GetOnBossEnemyDead() { return OnBossEnemyDead; }

	virtual void SetIsDead(const bool InbIsDead) override;

protected:
	virtual void BeginPlay() override;


private:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HideBossEnemyHpWidget();

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNetherCrownEnemyRangedBasicAttackComponent> RangedBasicAttackComponent{};

	FOnBossEnemyDead OnBossEnemyDead{};
};
