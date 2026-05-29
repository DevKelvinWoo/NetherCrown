// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownEnemySkillObject.h"
#include "NetherCrownEnemyTeleport.generated.h"

UCLASS(Blueprintable)
class NETHERCROWN_API UNetherCrownEnemyTeleport : public UNetherCrownEnemySkillObject
{
	GENERATED_BODY()

protected:
	virtual void InitEnemySkillObject() override;
	virtual void PlayEnemySkillCosmetics() override;
	virtual void ExecuteEnemySkillGameplay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void PlayTeleportSound() const;
	void SpawnTeleportEffect() const;

	void CacheEnemyTeleportData();

	void TeleportToEscape();

	UPROPERTY(Transient, Replicated)
	FVector TeleportStartVector{};

	FNetherCrownEnemyTeleportData CachedTeleportData{};
};
