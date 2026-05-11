// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownEnemySkillObject.h"
#include "NetherCrownEnemyShockwaveRepulse.generated.h"

UCLASS(Blueprintable)
class NETHERCROWN_API UNetherCrownEnemyShockwaveRepulse : public UNetherCrownEnemySkillObject
{
	GENERATED_BODY()

protected:
	virtual void InitEnemySkillObject() override;
	virtual void PlayEnemySkillCosmetics() override;
	virtual void ExecuteEnemySkillGameplay() override;

private:
	void CacheEnemyShockwaveRepulseData();

	void StartShockwaveRepulseKnockbackTimer();
	void ApplyShockwaveRepulse();

	UPROPERTY(Transient)
	FNetherCrownEnemyShockwaveRepulseData CachedEnemyShockwaveRepulseData{};

	FTimerHandle ShockwaveRepulseKnockbackTimerHandle{};
};
