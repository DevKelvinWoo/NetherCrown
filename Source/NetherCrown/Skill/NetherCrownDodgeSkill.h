// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownSkillObject.h"
#include "NetherCrown/Data/NetherCrownSkillData.h"
#include "NetherCrownDodgeSkill.generated.h"

UCLASS(Blueprintable)
class NETHERCROWN_API UNetherCrownDodgeSkill : public UNetherCrownSkillObject
{
	GENERATED_BODY()

public:
	UNetherCrownDodgeSkill();

protected:
	virtual void InitSkillObject() override;
	virtual void ExecuteSkillGameplay() override;
	virtual void PlaySkillCosmetics() override;

private:
	void CacheDodgeSkillData();

	void DodgeCharacter();

	UPROPERTY(Transient)
	FNetherCrownDodgeSkillData DodgeSkillData{};
};
