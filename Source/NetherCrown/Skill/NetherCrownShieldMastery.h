// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownSkillObject.h"
#include "NetherCrownShieldMastery.generated.h"

class ANetherCrownShield;
class UNiagaraComponent;

USTRUCT()
struct FNetherCrownShieldMasteryData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "SkillData")
	int32 ShieldDuration{ 10 };

	UPROPERTY(EditDefaultsOnly, Category = "SkillData")
	int32 ShieldValue{ 50 };

	UPROPERTY(EditDefaultsOnly, Category = "SkillData")
	FName ShieldEffectSocketName{ TEXT("root") };

	UPROPERTY(EditDefaultsOnly, Category = "ShieldActor")
	TSubclassOf<ANetherCrownShield> ShieldActorClass{};
};

UCLASS(Blueprintable)
class NETHERCROWN_API UNetherCrownShieldMastery : public UNetherCrownSkillObject
{
	GENERATED_BODY()

public:
	UNetherCrownShieldMastery();

protected:
	virtual void PlaySkillCosmetics() override;
	virtual void ExecuteSkillGameplay() override;

private:
	void ActiveShieldEffectAndActor();
	void DeactivateShieldEffectAndActor();

	void PlayShieldOnSound() const;

	void AddPlayerShieldAndSetShieldEndTimer(int32 InShieldValue) const;
	void ClearPlayerShield() const;

	UPROPERTY(EditDefaultsOnly, Category = "SkillData")
	FNetherCrownShieldMasteryData ShieldMasteryData{};

	UPROPERTY(Transient)
	TObjectPtr<UNiagaraComponent> HandledShieldMasteryNiagaraComponent{};

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownShield> HandledShieldMasteryActor{};
};
