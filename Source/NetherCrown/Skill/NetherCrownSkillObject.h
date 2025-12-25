// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrownSkillObject.generated.h"

class UAnimMontage;

class ANetherCrownCharacter;

UENUM()
enum class ENetherCrownSkillKeyEnum : uint8
{
	QSkill,
	WSkill,
	ESkill,
	RSkill,
	ShiftSkill
};

UCLASS(Abstract)
class NETHERCROWN_API UNetherCrownSkillObject : public UObject
{
	GENERATED_BODY()

public:
	virtual void ActiveSkill() {};

	ENetherCrownSkillKeyEnum GetSkillEnum() const { return SkillKeyEnum; }

	void SetSkillOwnerCharacter(ANetherCrownCharacter* SkillOwnerCharacter) { SkillOwnerCharacterWeak = MakeWeakObjectPtr(SkillOwnerCharacter); }

protected:
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> SkillAnimMontageSoft{};

	TWeakObjectPtr<ANetherCrownCharacter> SkillOwnerCharacterWeak{};

private:
	UPROPERTY(EditDefaultsOnly)
	ENetherCrownSkillKeyEnum SkillKeyEnum{};
};
