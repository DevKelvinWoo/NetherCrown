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

public:
	ANetherCrownBossEnemy();

	UNetherCrownEnemyRangedBasicAttackComponent* GetRangedBasicAttackComponent() const { return RangedBasicAttackComponent; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNetherCrownEnemyRangedBasicAttackComponent> RangedBasicAttackComponent{};
};
