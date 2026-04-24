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

UENUM()
enum class ENetherCrownSkillKeyEnum : uint8
{
	None,
	QSkill,
	ESkill,
	RSkill,
	ShiftSkill,
	CSkill
};

USTRUCT()
struct FNetherCrownSkillTagData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag SkillHitImpactEffectTag{};
};

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
	float SkillCooldown{ 5.f };

	UPROPERTY(EditDefaultsOnly)
	float SkillMPCost{ 20.f };

	UPROPERTY(EditDefaultsOnly)
	ENetherCrownSkillKeyEnum SkillKeyEnum{ ENetherCrownSkillKeyEnum::None };

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> SkillAnimMontageSoft{};

	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FNetherCrownSkillTagData SkillTagData{};

	UPROPERTY(EditDefaultsOnly)
	float SkillHitTime{ -1.f };

	UPROPERTY(EditDefaultsOnly)
	float SkillStartTimeOffset{ -1.f };

	UPROPERTY(EditDefaultsOnly)
	float SkillEndTimeOffset{ -1.f };

	UPROPERTY(EditDefaultsOnly)
	float CharacterMovementFlyTimeOffset{ -1.f };

	UPROPERTY(EditDefaultsOnly)
	float CharacterMovementWalkTimeOffset{ -1.f };

	UPROPERTY(EditDefaultsOnly)
	float SkillAuraActiveTimeOffset{ -1.f };

	UPROPERTY(EditDefaultsOnly)
	float SkillAuraDeactivateTimeOffset{ -1.f };

	UPROPERTY(EditDefaultsOnly)
	float SkillMontageBeginSlowPlayRate{ 0.5f };

	UPROPERTY(EditDefaultsOnly)
	float SkillMontageBeginSlowTimeOffset{ -1.f };

	UPROPERTY(EditDefaultsOnly)
	float SkillMontageEndSlowPlayRate{ 1.f };

	UPROPERTY(EditDefaultsOnly)
	float SkillMontageEndSlowTimeOffset{ -1.f };

	UPROPERTY(EditDefaultsOnly)
	float SkillCoolDownDecreaseOffset{ 0.1f };
};

UCLASS(Abstract)
class NETHERCROWN_API UNetherCrownSkillObject : public UObject
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSkillCoolDownModified, const float, const ENetherCrownSkillKeyEnum);

public:
	virtual void InitSkillObject();
	virtual void PlaySkillCosmetics();
	virtual void ExecuteSkillGameplay();

	//@NOTE : Timeline
	virtual void TickFloatTimeline(float DeltaTime){};

	ENetherCrownSkillKeyEnum GetSkillEnum() const;
	const FNetherCrownSkillData& GetSkillData() const;

	void SetSkillOwnerCharacter(ANetherCrownCharacter* SkillOwnerCharacter);
	void SetSkillTag(const FGameplayTag& InSkillTag) { SkillTag = InSkillTag; }

	bool IsSkillCoolDown() const { return bIsCoolDown; }

	FOnSkillCoolDownModified& GetOnSkillCoolDownModified() { return OnSkillCoolDownModified; }

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

	int32 CalculateSkillDamage() const;

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
	void CacheSkillData();

	const TSoftObjectPtr<UAnimMontage>& GetSkillAnimMontageSoft() const;
	const FGameplayTag& GetSkillHitImpactEffectTag() const;
	float GetSkillHitTime() const;
	float GetSkillStartTimeOffset() const;
	float GetSkillEndTimeOffset() const;
	float GetCharacterMovementFlyTimeOffset() const;
	float GetCharacterMovementWalkTimeOffset() const;
	float GetSkillAuraActiveTimeOffset() const;
	float GetSkillAuraDeactivateTimeOffset() const;
	float GetSkillMontageBeginSlowPlayRate() const;
	float GetSkillMontageBeginSlowTimeOffset() const;
	float GetSkillMontageEndSlowPlayRate() const;
	float GetSkillMontageEndSlowTimeOffset() const;
	float GetSkillCoolDownDecreaseOffset() const;

	UPROPERTY(Replicated)
	TWeakObjectPtr<ANetherCrownCharacter> SkillOwnerCharacterWeak{};

	UPROPERTY(Replicated)
	FNetherCrownSkillData SkillData{};

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

	FTimerHandle SkillCoolDownTimerHandle{};

private:
	void SetSkillMontageSlowPlayRate(float InPlayRate) const;

	void StartSkillCoolDownTimer();
	void StopSkillCoolDownTimer();

	UFUNCTION(Client, Unreliable)
	void Client_SkillCoolDownModify(const float CoolDownRatio);

	UPROPERTY(EditDefaultsOnly, Replicated)
	FGameplayTag SkillTag{};

	UPROPERTY(Replicated)
	bool bIsCoolDown{ false };

	UPROPERTY()
	float SkillCoolDownAccumulator{ 0.f };

	FOnSkillCoolDownModified OnSkillCoolDownModified;
};
