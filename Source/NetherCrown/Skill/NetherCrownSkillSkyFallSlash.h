// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownSkillObject.h"
#include "NetherCrownSkillSkyFallSlash.generated.h"

class UCurveFloat;

UCLASS(Blueprintable)
class NETHERCROWN_API UNetherCrownSkillSkyFallSlash : public UNetherCrownSkillObject
{
	GENERATED_BODY()

protected:
	virtual void ExecuteSkillGameplay() const override;
	virtual void PlaySkillCosmetics() override;

private:
	void ApplySkillCameraCurveFloat();
	void StartSkillCameraCurveTimer();

	FTimerHandle SkillCameraCurveTimerHandle;
	float ElapsedTime{};

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UCurveFloat> SkillCameraCurveFloatSoft{};
};
