// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownEnemySkillObject.h"
#include "NetherCrownEnemyVoidPiercer.generated.h"

UCLASS(Blueprintable)
class NETHERCROWN_API UNetherCrownEnemyVoidPiercer : public UNetherCrownEnemySkillObject
{
	GENERATED_BODY()

protected:
	virtual void InitEnemySkillObject() override;
	virtual void PlayEnemySkillCosmetics() override;
	virtual void ExecuteEnemySkillGameplay() override;

private:
	void StartVoidPiercerTraceTimer();
	void AttackVoidPiercerToCharacter();

	void CacheVoidPiercerData();

	void JumpAndFlyToUseSkill();
	void RotateToTargetActor();

	void HandleVoidPiercerJumpFinished();
	void HandleVoidPiercerFlyFinished();

	UPROPERTY(Transient)
	FNetherCrownEnemyVoidPiercerData CachedVoidPiercerData{};

	FTimerHandle FlyTimerHandle{};
	FTimerHandle LandTimerHandle{};
	FTimerHandle TraceTimerHandle{};

	bool bCanActiveTrace{ false };
};
