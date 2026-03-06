// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "NetherCrownBasicAttackComponent.generated.h"

class ANetherCrownCharacter;
class UAnimMontage;
class UCameraShakeBase;

class ANetherCrownWeapon;

USTRUCT()
struct FNetherCrownBasicAttackComponentTagData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag BasicAttackGruntSoundTag{};

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag BasicAttackImpactEffectTag{};
};

USTRUCT()
struct FNetherCrownComboTimingData
{
	GENERATED_BODY()

public:
	//@NOTE : Montage Section 시작 기준, 콤보 윈도우가 열리는 시간 (초)
	UPROPERTY(EditDefaultsOnly)
	float ComboWindowOpenTime = 0.0f;

	//@NOTE : Montage Section 시작 기준, 콤보 윈도우가 닫히는 시간 (초)
	UPROPERTY(EditDefaultsOnly)
	float ComboWindowCloseTime = 0.0f;
};

UENUM()
enum class ENetherCrownBasicAttackState : uint8
{
	CanAttack,
	CannotAttack,
	Attacking,
	NotAttacking,
	CanQueueNextCombo,
	CannotQueueNextCombo,
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

	void ApplyDamageToHitEnemy(AActor* HitEnemy, const FVector& HitLocation);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void CacheBasicAttackMontage();
	void CacheCharacter();

	void CalculateNextComboCount();

	void StartAttackBasic();

	void PlayAttackSoundAndJumpToComboMontageSection(const FName* SectionName);

	void SetEquippedWeaponTraceEnable(const bool bEnable) const;

	UFUNCTION(Client, Reliable)
	void Client_InitWeaponTraceComponentSettings();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AutoTargetEnemy();

	void AutoTargetEnemy() const;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAndJumpToComboMontageSection(const FName& SectionName);

	UFUNCTION(Server, Reliable)
	void Server_RequestBasicAttack();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayHitImpactEffect(const FVector& HitLocation);

	UFUNCTION(Client, Unreliable)
	void Client_PlayHitImpactCameraShake();

	UFUNCTION()
	void ApplyDamageInternal(AActor* HitEnemy) const;

	void HandleOnEquipWeapon(const bool bEquipWeapon);

	int32 CalculateBasicAttackDamage() const;

	void PlayHitImpactCameraShake() const;
	void PlayBasicAttackSounds() const;
	void SpawnHitImpactEffect(const FVector& HitLocation) const;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> BasicAttackAnimMontageSoft{};

	UPROPERTY(EditDefaultsOnly)
	TMap<int32, FName> ComboMontageSectionMap{};

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCameraShakeBase> ApplyDamageCameraShakeClass{};

	UPROPERTY(EditDefaultsOnly)
	float AutoTargetingRadius{ 130.0f };

	int32 CurrentComboCount{ 1 };

	UPROPERTY(Replicated)
	ENetherCrownBasicAttackState BasicAttackState{ ENetherCrownBasicAttackState::CannotAttack };

	UPROPERTY(EditDefaultsOnly)
	FNetherCrownBasicAttackComponentTagData BasicAttackComponentTagData{};

	UPROPERTY()
	TObjectPtr<UAnimMontage> CachedBasicAttackMontage{};

	UPROPERTY()
	TObjectPtr<ANetherCrownCharacter> CachedCharacter{};

	FOnStopOrStartBasicAttackAnim OnStopOrStartBasicAttackAnim;

	void SetupBasicAttackTimers(const int32 ComboCount);
	void ServerHandleComboWindowOpen();
	void ServerHandleComboWindowClose();
	void ServerHandleAttackEnd();
	void ServerHandleHitTraceEnable();

	UPROPERTY(EditDefaultsOnly)
	TMap<int32, FNetherCrownComboTimingData> ComboTimingDataMap{};

	UPROPERTY(EditDefaultsOnly)
	TMap<int32, float> AttackEndTimingDataMap{};

	UPROPERTY(EditDefaultsOnly)
	TMap<int32, float> HitTraceEnableTimingDataMap{};

	FTimerHandle ComboWindowOpenTimerHandle;
	FTimerHandle ComboWindowCloseTimerHandle;
	FTimerHandle AttackEndTimerHandle;
	FTimerHandle HitTraceEnableHandle;
};
