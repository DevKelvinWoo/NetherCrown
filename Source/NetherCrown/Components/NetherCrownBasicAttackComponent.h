// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrown/Data/NetherCrownBasicAttackData.h"
#include "NetherCrownBasicAttackComponent.generated.h"

class UAnimMontage;

class ANetherCrownWeapon;
class ANetherCrownEnemy;
class ANetherCrownCharacter;
class UNetherCrownCharacterAnimInstance;

enum class ENetherCrownCrowdControlType : uint8;

UENUM()
enum class ENetherCrownBasicAttackState : uint8
{
	CanAttack,
	CannotAttack,
	Attacking,
	CanQueueNextCombo,
	ComboQueued
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownBasicAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownBasicAttackComponent();

	void RequestBasicAttack();

	void SetCanAttack(const bool InbCanAttack);

	bool IsAttacking() const;

	void ApplyHitCosmeticAndDamageToHitEnemy(ANetherCrownEnemy* HitEnemy, const FVector& HitLocation);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void LoadBasicAttackData();
	void CacheBasicAttackMontage();
	void CacheCharacter();

	UNetherCrownCharacterAnimInstance* GetOwnerCharacterAnimInstance() const;

	void CalculateNextComboCount();
	int32 CalculateBasicAttackDamage() const;

	void StartAttackBasic();

	void PlayAndJumpToComboMontageSection(const FName* SectionName, const bool bIsLastCombo = false);

	void SetEquippedWeaponTraceEnable(const bool bEnable) const;

	UFUNCTION(Client, Reliable)
	void Client_InitWeaponTraceComponentSettings();

	UFUNCTION(Client, Unreliable)
	void Client_PlayHitImpactCameraShake();

	UFUNCTION(Client, Unreliable)
	void Client_StartBasicAttackHitStop();

	void ApplyBasicAttackPlayRate(const float AnimRate);
	void RestoreBasicAttackPlayRate();

	UFUNCTION(Client, Unreliable)
	void Client_StartBasicAttackPushIn();

	void ApplyLastComboHitPP();
	void SetupLastComboAnimRateTimer();
	void SetupLastComboAttackAuraTimer();

	void ApplyLastComboAttackKnockBack(const ANetherCrownEnemy* HitEnemy);

	void SetLastComboAttackWeaponAura(const bool bIsActivate);
	void ActiveLastComboAttackWeaponAura();
	void DeactivateLastComboAttackWeaponAura();

	void ApplyBasicAttackPushIn();
	void RestoreBasicAttackPushIn();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AutoTargetEnemy();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAndJumpToComboMontageSection(const FName& SectionName, const bool bIsLastCombo = false);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayHitImpactEffect(const FVector& HitLocation, const ANetherCrownEnemy* HitEnemy);

	UFUNCTION(Server, Reliable)
	void Server_RequestBasicAttack();

	void AutoTargetEnemy() const;

	UFUNCTION()
	void ApplyDamageInternal(AActor* HitEnemy) const;

	void HandleOnEquipWeapon(const bool bEquipWeapon);

	void PlayHitImpactCameraShake() const;
	void SpawnHitImpactEffect(const FVector& HitLocation, const ANetherCrownEnemy* HitEnemy) const;

	void SetupBasicAttackTimers(const int32 ComboCount);
	void ServerHandleComboWindowOpen();
	void ServerHandleComboWindowClose();
	void ServerHandleAttackEnd();
	void ServerHandleHitTraceEnable();

	void SetWeaponTraceMode();
	bool IsLastComboAttack() const;
	int32 GetMaxComboCount() const;

	UFUNCTION()
	void HandleCurrentComboCountReplicated();

	UPROPERTY(Transient)
	FNetherCrownBasicAttackData BasicAttackData{};

	UPROPERTY(EditDefaultsOnly, Category = "BasicAttackDataAsset")
	TSoftObjectPtr<UNetherCrownBasicAttackDataAsset> BasicAttackDataAssetSoft{};

	UPROPERTY(ReplicatedUsing=HandleCurrentComboCountReplicated)
	int32 CurrentComboCount{ 1 };

	UPROPERTY(Replicated)
	ENetherCrownBasicAttackState BasicAttackState{ ENetherCrownBasicAttackState::CannotAttack };

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CachedBasicAttackMontage{};

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownCharacter> CachedCharacter{};

	FTimerHandle ComboWindowOpenTimerHandle;
	FTimerHandle ComboWindowCloseTimerHandle;
	FTimerHandle AttackEndTimerHandle;
	FTimerHandle HitTraceEnableHandle;
	FTimerHandle HitStopTimer;
	FTimerHandle PushInTimerHandle;
	FTimerHandle LastComboAttackAuraTimer;

	float CachedMainSpringArmLengthBeforePushIn{ 0.f };
};
