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
	ENetherCrownSkillKeyEnum GetSkillEnum() const { return SkillKeyEnum; }

	void SetSkillOwnerCharacter(ANetherCrownCharacter* SkillOwnerCharacter) { SkillOwnerCharacterWeak = MakeWeakObjectPtr(SkillOwnerCharacter); }

	virtual void PlaySkillCosmetics() const;
	virtual void ExecuteSkillGameplay() const {};

	//@TODO : virtual함수로 스킬의 서버로직 (데미지 등)을 구현하는 함수 선언 후 자식에서 구현

protected:
	//@NOTE : To Replicate UObject
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> SkillAnimMontageSoft{};

	UPROPERTY(Replicated)
	TWeakObjectPtr<ANetherCrownCharacter> SkillOwnerCharacterWeak{};

private:
	UPROPERTY(EditDefaultsOnly, Replicated)
	ENetherCrownSkillKeyEnum SkillKeyEnum{};
};
