// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownCrowdControlComponent.h"
#include "NetherCrownSkillObject.generated.h"

class UAnimMontage;

class ANetherCrownCharacter;
class ANetherCrownEnemy;

USTRUCT()
struct FNetherCrownSkillData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FString SkillName{};

	UPROPERTY(EditDefaultsOnly)
	FString SkillDescription{};

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> SkillIconTextureSoft{};

	UPROPERTY(EditDefaultsOnly)
	int32 SkillDamage{ 100 };

	UPROPERTY(EditDefaultsOnly)
	int32 SkillCooldown{ 5 };
};

USTRUCT()
struct FNetherCrownSkillEffectTagData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag SkillHitImpactEffectTag{};
};

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
	virtual void ExecuteSkillGameplay();

	void SetSkillMontageSlowPlayRate(float InPlayRate) const;

	float GetSkillMontageBeginSlowPlayRate() const { return SkillMontageBeginSlowPlayRate; }
	float GetSkillMontageEndSlowPlayRate() const { return SkillMontageEndSlowPlayRate; }

	void StartSkillCoolDownTimer();
	void StopSkillCoolDownTimer();

	bool CanActiveSkill() const { return bCanActiveSkill; }

protected:
	//@NOTE : To Replicate UObject
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	//@NOTE : To call RPC function in UObject
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack) override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnSkillImpactEffect(const ANetherCrownEnemy* TargetEnemy) const;

	//CC
	void ApplyCrowdControlToTarget(ANetherCrownEnemy* TargetEnemy, const ENetherCrownCrowdControlType InCrowdControlType, const float CrowdControlDuration);

	void PlayEnemyHitSound(const ANetherCrownEnemy* TargetEnemy) const;
	void PlaySkillHitImpactEffect(const ANetherCrownEnemy* TargetEnemy) const;

	int32 CalculatePhysicalSkillDamage() const;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> SkillAnimMontageSoft{};

	UPROPERTY(Replicated)
	TWeakObjectPtr<ANetherCrownCharacter> SkillOwnerCharacterWeak{};

	UPROPERTY(EditDefaultsOnly)
	FNetherCrownSkillEffectTagData SkillEffectTagData{};

	UPROPERTY(EditDefaultsOnly)
	FNetherCrownSkillData SkillData{};

private:
	UPROPERTY(EditDefaultsOnly, Replicated)
	ENetherCrownSkillKeyEnum SkillKeyEnum{};

	UPROPERTY(EditDefaultsOnly, Replicated)
	float SkillMontageBeginSlowPlayRate{ 0.5f };

	UPROPERTY(EditDefaultsOnly, Replicated)
	float SkillMontageEndSlowPlayRate{ 1.f };

	UPROPERTY(EditDefaultsOnly, Replicated)
	bool bCanActiveSkill{ true };

	UPROPERTY(EditDefaultsOnly, Replicated)
	int32 SkillCoolDown{ 3 };
};
