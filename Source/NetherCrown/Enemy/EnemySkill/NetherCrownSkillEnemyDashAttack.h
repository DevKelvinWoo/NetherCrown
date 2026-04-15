// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownEnemySkillObject.h"
#include "NetherCrown/Data/NetherCrownEnemySkillData.h"
#include "NetherCrownSkillEnemyDashAttack.generated.h"

class ANetherCrownEnemy;
class ANetherCrownCharacter;

UCLASS(Blueprintable)
class NETHERCROWN_API UNetherCrownSkillEnemyDashAttack : public UNetherCrownEnemySkillObject
{
	GENERATED_BODY()

public:
	void EnemyDashAttack();

protected:
	virtual void InitEnemySkillObject() override;

	virtual void PlayEnemySkillCosmetics() override;
	virtual void ExecuteEnemySkillGameplay() override;

private:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EnemyDashAttack();

	void CacheEnemyDashAttackData();

	UPROPERTY(Transient)
	FNetherCrownEnemyDashAttackData CachedEnemyDashAttackData{};
};
