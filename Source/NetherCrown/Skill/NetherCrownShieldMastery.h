// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownSkillObject.h"
#include "NetherCrownShieldMastery.generated.h"

class UNiagaraComponent;

UCLASS(Blueprintable)
class NETHERCROWN_API UNetherCrownShieldMastery : public UNetherCrownSkillObject
{
	GENERATED_BODY()

public:
	UNetherCrownShieldMastery();

protected:
	virtual void InitSkillObject() override;
	virtual void PlaySkillCosmetics() override;
	virtual void ExecuteSkillGameplay() override;

private:
	void CacheShieldMasteryData();

	void ActiveShieldEffectAndActor();
	void DeactivateShieldEffectAndActor();

	void PlayShieldOnSound() const;

	void AddPlayerShieldAndSetShieldEndTimer(const int32 InShieldValue);
	void ClearPlayerShield();

	FNetherCrownShieldMasteryData ShieldMasteryData{};

	UPROPERTY(Transient)
	TObjectPtr<UNiagaraComponent> HandledShieldMasteryNiagaraComponent{};

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownShield> HandledShieldMasteryActor{};

	FTimerHandle ShieldMasteryDeactivateTimerHandle{};
	FTimerHandle ShieldMasteryClearPlayerShieldTimerHandle{};
};
