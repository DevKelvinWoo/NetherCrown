// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrown/Data/NetherCrownBasicAttackData.h"
#include "NetherCrownBasicAttackComponent.generated.h"

class ANetherCrownCharacter;
class UAnimMontage;

class ANetherCrownWeapon;
class UNetherCrownCharacterAnimInstance;

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

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnStopOrStartBasicAttackAnim, const bool);

public:
	UNetherCrownBasicAttackComponent();

	void RequestBasicAttack();

	void SetCanAttack(const bool InbCanAttack);

	bool IsAttacking() const { return BasicAttackState == ENetherCrownBasicAttackState::Attacking; }

	FOnStopOrStartBasicAttackAnim& GetOnStopOrStartBasicAttack() { return OnStopOrStartBasicAttackAnim; }

	void ApplyHitCosmeticAndDamageToHitEnemy(AActor* HitEnemy, const FVector& HitLocation);

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

	void PlayAttackSoundAndJumpToComboMontageSection(const FName* SectionName);

	void SetEquippedWeaponTraceEnable(const bool bEnable) const;

	UFUNCTION(Client, Reliable)
	void Client_InitWeaponTraceComponentSettings();

	UFUNCTION(Client, Unreliable)
	void Client_PlayHitImpactCameraShake();

	UFUNCTION(Client, Unreliable)
	void Client_StartBasicAttackHitStop();

	void ApplyBasicAttackHitStop();
	void RestoreBasicAttackPlayRate();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AutoTargetEnemy();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAndJumpToComboMontageSection(const FName& SectionName);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayHitImpactEffect(const FVector& HitLocation);

	UFUNCTION(Server, Reliable)
	void Server_RequestBasicAttack();

	void AutoTargetEnemy() const;

	UFUNCTION()
	void ApplyDamageInternal(AActor* HitEnemy) const;

	void HandleOnEquipWeapon(const bool bEquipWeapon);

	void PlayHitImpactCameraShake() const;
	void PlayBasicAttackSounds() const;
	void SpawnHitImpactEffect(const FVector& HitLocation) const;

	void SetupBasicAttackTimers(const int32 ComboCount);
	void ServerHandleComboWindowOpen();
	void ServerHandleComboWindowClose();
	void ServerHandleAttackEnd();
	void ServerHandleHitTraceEnable();

	UPROPERTY(Transient)
	FNetherCrownBasicAttackData BasicAttackData{};

	UPROPERTY(EditDefaultsOnly, Category = "BasicAttackDataAsset")
	TSoftObjectPtr<UNetherCrownBasicAttackDataAsset> BasicAttackDataAssetSoft{};

	int32 CurrentComboCount{ 1 };

	UPROPERTY(Replicated)
	ENetherCrownBasicAttackState BasicAttackState{ ENetherCrownBasicAttackState::CannotAttack };

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CachedBasicAttackMontage{};

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownCharacter> CachedCharacter{};

	FOnStopOrStartBasicAttackAnim OnStopOrStartBasicAttackAnim;

	FTimerHandle ComboWindowOpenTimerHandle;
	FTimerHandle ComboWindowCloseTimerHandle;
	FTimerHandle AttackEndTimerHandle;
	FTimerHandle HitTraceEnableHandle;
	FTimerHandle HitStopTimer;
};
