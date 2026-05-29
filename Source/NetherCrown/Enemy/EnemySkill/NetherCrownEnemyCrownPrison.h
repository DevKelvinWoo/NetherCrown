// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownEnemySkillObject.h"
#include "NetherCrownEnemyCrownPrison.generated.h"

class ANetherCrownEnemyCrownPrisonWall;

UCLASS(Blueprintable)
class NETHERCROWN_API UNetherCrownEnemyCrownPrison : public UNetherCrownEnemySkillObject
{
	GENERATED_BODY()

protected:
	virtual void InitEnemySkillObject() override;
	virtual void PlayEnemySkillCosmetics() override;
	virtual void ExecuteEnemySkillGameplay() override;

private:
	void CacheEnemyCrownPrisonData();

	void SpawnCrownPrisonWall();
	FVector GetCrownPrisonWallSpawnLocation() const;

	void HandleOnCrownPrisonExplosionHit(ANetherCrownCharacter* HitCharacter);

	FNetherCrownEnemyCrownPrisonData CachedCrownEnemyCrownPrisonData{};
};
