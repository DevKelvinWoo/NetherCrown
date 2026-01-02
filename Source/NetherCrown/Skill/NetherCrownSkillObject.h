// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrownSkillObject.generated.h"

class UAnimMontage;

class ANetherCrownCharacter;
class ANetherCrownEnemy;

UENUM()
enum class ENetherCrownSkillKeyEnum : uint8
{
	None,
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
	virtual void InitSkillObject() {};

	ENetherCrownSkillKeyEnum GetSkillEnum() const { return SkillKeyEnum; }

	void SetSkillOwnerCharacter(ANetherCrownCharacter* SkillOwnerCharacter) { SkillOwnerCharacterWeak = MakeWeakObjectPtr(SkillOwnerCharacter); }

	virtual void PlaySkillCosmetics();
	virtual void ExecuteSkillGameplay() const {};

	void SetSkillMontageSlowPlayRate(float InPlayRate) const;

	float GetSkillMontageBeginSlowPlayRate() const { return SkillMontageBeginSlowPlayRate; }
	float GetSkillMontageEndSlowPlayRate() const { return SkillMontageEndSlowPlayRate; }

protected:
	//@NOTE : To Replicate UObject
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	//CC
	void ApplyKnockBackToTarget(ACharacter* TargetCharacter, const FVector& KnockBackVector);

	void PlayEnemyHitSound(const ANetherCrownEnemy* TargetEnemy) const;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> SkillAnimMontageSoft{};

	UPROPERTY(Replicated)
	TWeakObjectPtr<ANetherCrownCharacter> SkillOwnerCharacterWeak{};

private:
	UPROPERTY(EditDefaultsOnly, Replicated)
	ENetherCrownSkillKeyEnum SkillKeyEnum{};

	UPROPERTY(EditDefaultsOnly, Replicated)
	float SkillMontageBeginSlowPlayRate{ 0.5f };

	UPROPERTY(EditDefaultsOnly, Replicated)
	float SkillMontageEndSlowPlayRate{ 1.f };
};
