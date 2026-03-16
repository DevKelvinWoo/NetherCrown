// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "NetherCrownSkillObject.generated.h"

class UAnimMontage;

class ANetherCrownCharacter;
class ANetherCrownEnemy;
enum class ENetherCrownCrowdControlType : uint8;
enum class ENetherCrownPPType : uint8;

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

	UPROPERTY(EditDefaultsOnly)
	float SkillMPCost{ 20.f };
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
	ESkill,
	RSkill,
	ShiftSkill
};

UCLASS(Abstract)
class NETHERCROWN_API UNetherCrownSkillObject : public UObject
{
	GENERATED_BODY()

public:
	virtual void InitSkillObject();
	virtual void PlaySkillCosmetics();
	virtual void ExecuteSkillGameplay();

	//@NOTE : Timeline
	virtual void TickFloatTimeline(float DeltaTime){};

	ENetherCrownSkillKeyEnum GetSkillEnum() const { return SkillKeyEnum; }

	void SetSkillOwnerCharacter(ANetherCrownCharacter* SkillOwnerCharacter) { SkillOwnerCharacterWeak = MakeWeakObjectPtr(SkillOwnerCharacter); }

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
	void ApplyCrowdControlToTarget(const ANetherCrownEnemy* TargetEnemy, const ENetherCrownCrowdControlType InCrowdControlType, const float CrowdControlDuration);

	void PlaySkillHitImpactEffect(const ANetherCrownEnemy* TargetEnemy) const;

	int32 CalculatePhysicalSkillDamage() const;

	void ApplyPostProcess(const ENetherCrownPPType PPType, float Duration, const bool bEndTimerAutomatic = true) const;

	void SetupSkillStateTimer();
	void SetupSkillMovementModeTimer();
	void SetupSkillSlowTimer();
	void SetupSkillWeaponAuraTimer();

	void StartSkillState();
	void EndSkillState();

	void MakeSkillAnimationSlowly();
	void RestoreSkillAnimationPlayRate();

	void SetCharacterMovementFly();
	void SetCharacterMovementWalk();

	void SetSkillWeaponAura(const bool bIsActivate);
	void ActiveSkillWeaponAura();
	void DeactivateSkillWeaponAura();

	void SpendMP();

	UPROPERTY(EditDefaultsOnly, Category = "SkillAnimMontage")
	TSoftObjectPtr<UAnimMontage> SkillAnimMontageSoft{};

	UPROPERTY(Transient, Replicated)
	TWeakObjectPtr<ANetherCrownCharacter> SkillOwnerCharacterWeak{};

	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FNetherCrownSkillEffectTagData SkillEffectTagData{};

	UPROPERTY(EditDefaultsOnly, Replicated, Category = "SkillData")
	FNetherCrownSkillData SkillData{};

	UPROPERTY(EditDefaultsOnly, Category = "SkillTimerData")
	float SkillHitTime{ -1.f };

	UPROPERTY(EditDefaultsOnly, Category = "SkillTimerData")
	float SkillStartTimeOffset{ -1.f };

	UPROPERTY(EditDefaultsOnly, Category = "SkillTimerData")
	float SkillEndTimeOffset{ -1.f };

	UPROPERTY(EditDefaultsOnly, Category = "SkillTimerData")
	float CharacterMovementFlyTimeOffset{ -1.f };

	UPROPERTY(EditDefaultsOnly, Category = "SkillTimerData")
	float CharacterMovementWalkTimeOffset{ -1.f };

	UPROPERTY(EditDefaultsOnly, Category = "SkillTimerData")
	float SkillAuraActiveTimeOffset{ -1.f };

	UPROPERTY(EditDefaultsOnly, Category = "SkillTimerData")
	float SkillAuraDeactivateTimeOffset{ -1.f };

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CachedSkillAnimMontage{};

	FTimerHandle SkillHitTimerHandle{};

	FTimerHandle SkillStartTimerHandle{};
	FTimerHandle SkillEndTimerHandle{};

	FTimerHandle SkillAnimationSlowStartTimerHandle{};
	FTimerHandle SkillAnimationSlowEndTimerHandle{};

	FTimerHandle CharacterMovementFlyTimerHandle{};
	FTimerHandle CharacterMovementWalkTimerHandle{};

	FTimerHandle SkillWeaponAuraActiveTimerHandle{};
	FTimerHandle SkillWeaponAuraDeactivateTimerHandle{};

private:
	void SetSkillMontageSlowPlayRate(float InPlayRate) const;

	void StartSkillCoolDownTimer();
	void StopSkillCoolDownTimer();

	UPROPERTY(EditDefaultsOnly, Replicated)
	ENetherCrownSkillKeyEnum SkillKeyEnum{};

	UPROPERTY(EditDefaultsOnly, Category = "SkillTimerData")
	float SkillMontageBeginSlowPlayRate{ 0.5f };

	UPROPERTY(EditDefaultsOnly, Category = "SkillTimerData")
	float SkillMontageBeginSlowTimeOffset{ -1.f };

	UPROPERTY(EditDefaultsOnly, Category = "SkillTimerData")
	float SkillMontageEndSlowPlayRate{ 1.f };

	UPROPERTY(EditDefaultsOnly, Category = "SkillTimerData")
	float SkillMontageEndSlowTimeOffset{ -1.f };

	UPROPERTY(EditDefaultsOnly, Replicated, Category = "SkillData")
	bool bCanActiveSkill{ true };
};
