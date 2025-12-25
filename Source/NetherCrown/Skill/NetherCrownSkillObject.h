// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NetherCrownSkillObject.generated.h"

UENUM()
enum class ENetherCrownSkillEnum : uint8
{
	QSkill,
	WSkill,
	ESkill,
	RSkill,
	ShiftSkill
};

UCLASS()
class NETHERCROWN_API UNetherCrownSkillObject : public UObject
{
	GENERATED_BODY()

public:
	ENetherCrownSkillEnum GetSkillEnum() const { return SkillEnum; }

	virtual void ActiveSkill() {};

private:
	UPROPERTY(EditDefaultsOnly)
	ENetherCrownSkillEnum SkillEnum{};
};
