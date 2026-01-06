// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownSkillObject.h"
#include "NetherCrownShieldMastery.generated.h"

class ANetherCrownShield;
class UNiagaraComponent;

UCLASS(Blueprintable)
class NETHERCROWN_API UNetherCrownShieldMastery : public UNetherCrownSkillObject
{
	GENERATED_BODY()

protected:
	virtual void PlaySkillCosmetics() override;
	virtual void ExecuteSkillGameplay() override;

private:
	void ActiveShieldEffectAndActor();
	void DeactivateShieldEffectAndActor();

	UPROPERTY(EditDefaultsOnly)
	int32 ShieldDuration{ 10 };

	UPROPERTY(EditDefaultsOnly)
	FName ShieldEffectSocketName{ TEXT("root") };

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> HandledShieldMasteryNiagaraComponent{};

	UPROPERTY()
	TObjectPtr<ANetherCrownShield> HandledShieldMasteryActor{};

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ANetherCrownShield> ShieldActorClass{};
};
