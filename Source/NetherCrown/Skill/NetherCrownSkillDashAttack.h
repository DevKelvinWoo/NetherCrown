// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownSkillObject.h"
#include "NetherCrownSkillDashAttack.generated.h"

UCLASS(Blueprintable)
class NETHERCROWN_API UNetherCrownSkillDashAttack : public UNetherCrownSkillObject
{
	GENERATED_BODY()

protected:
	virtual void PlaySkillCosmetics() override;
	virtual void ExecuteSkillGameplay() override;

private:
	void DashOwnerCharacter() const;

	const float DashVelocity{ 1000.f };
};
