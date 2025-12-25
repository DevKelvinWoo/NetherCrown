// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownSkillObject.h"
#include "NetherCrownSkillSkyFallSlash.generated.h"

/**
 *
 */
UCLASS(Blueprintable)
class NETHERCROWN_API UNetherCrownSkillSkyFallSlash : public UNetherCrownSkillObject
{
	GENERATED_BODY()

protected:
	virtual void ActiveSkill() override;
};
