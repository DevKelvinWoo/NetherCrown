// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "NetherCrownBasicAttackComponent.generated.h"

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

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownBasicAttackComponent : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnStopOrStartBasicAttackAnim, const bool);

public:
	void RequestBasicAttack();

	void HandleEnableComboWindow();
	void HandleDisableComboWindow();
	void HandleEnableHitTrace();

	void SetCanAttack(const bool InbCanAttack) { bCanAttack = InbCanAttack; }

	FOnStopOrStartBasicAttackAnim& GetOnStopOrStartBasicAttack() { return OnStopOrStartBasicAttackAnim; }

	void ApplyDamageToHitEnemy(AActor* HitEnemy, const FVector& HitLocation);

protected:
	virtual void BeginPlay() override;

private:
	void CalculateNextComboCount();

	void StartAttackBasic();

	void PlayAttackSoundAndJumpToComboMontageSection(const FName* SectionName) const;

	void SetEquippedWeaponTraceEnable(const bool bEnable) const;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAndJumpToComboMontageSection(const FName& SectionName);

	UFUNCTION(Server, Reliable)
	void Server_RequestBasicAttack();

	UFUNCTION(Server, Reliable)
	void Server_ApplyDamageToHitEnemy(AActor* HitEnemy);

	UFUNCTION(Server, Reliable)
	void Server_SpawnHitImpactEffect(const FVector& HitLocation);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayHitImpactEffect(const FVector& HitLocation);

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

	int32 CurrentComboCount{ 1 };

	bool bCanQueueNextCombo{ false };
	bool bHasQueuedNextCombo{ false };
	bool bCanInputFirstAttack{ true };
	bool bCanAttack{ false };

	UPROPERTY(EditDefaultsOnly)
	FNetherCrownBasicAttackComponentTagData BasicAttackComponentTagData{};

	FOnStopOrStartBasicAttackAnim OnStopOrStartBasicAttackAnim;
};
