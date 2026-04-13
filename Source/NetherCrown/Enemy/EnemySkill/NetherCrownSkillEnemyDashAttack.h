// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownEnemySkillObject.h"
#include "NetherCrownSkillEnemyDashAttack.generated.h"

class ANetherCrownEnemy;
class ANetherCrownCharacter;

UCLASS(Blueprintable)
class NETHERCROWN_API UNetherCrownSkillEnemyDashAttack : public UNetherCrownEnemySkillObject
{
	GENERATED_BODY()

public:
	void EnemyDashAttack(const ANetherCrownCharacter* TargetCharacter);

protected:
	virtual void PlayEnemySkillCosmetics() override;
	virtual void ExecuteEnemySkillGameplay() override;
};
