// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownEnemySkillObject.h"
#include "NetherCrownEnemyMagicSpike.generated.h"

UCLASS(Blueprintable)
class NETHERCROWN_API UNetherCrownEnemyMagicSpike : public UNetherCrownEnemySkillObject
{
	GENERATED_BODY()

protected:
	virtual void InitEnemySkillObject() override;
	virtual void PlayEnemySkillCosmetics() override;
	virtual void ExecuteEnemySkillGameplay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void CacheMagicSpikeData();

	FVector GetMagicSpikePoint() const;

	void DetectAndApplyMagicSpikeDamage(const FVector& InMagicSpikePointVector);

	void StartMagicSpikeRangeTimer();
	void StartMagicSpikeAttackTimer(const FVector& InMagicSpikePointVector);

	void ShowMagicSpikeRange();
	void ShowMagicSpikeImpactAndApplyDamage(FVector InMagicSpikePointVector);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowMagicSpikeEffectAndPlaySound(const FGameplayTag& InEffectTag, const FGameplayTag& InSoundTag, const FVector& InMagicSpikePointVector);

	UPROPERTY(Transient)
	FNetherCrownMagicSpikeData CachedMagicSpikeData{};

	UPROPERTY(Replicated)
	int32 CurrentMagicSpikeCount{};

	FTimerHandle MagicSpikeRangeTimerHandle{};
};
