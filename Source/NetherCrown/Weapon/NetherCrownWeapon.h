// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "NetherCrownWeapon.generated.h"

enum class ENetherCrownTraceMode : uint8;
class UNiagaraSystem;
class UBoxComponent;
class USphereComponent;
class USkeletalMeshComponent;
class UNiagaraComponent;

class UNetherCrownWeaponData;
class UNetherCrownWeaponTraceComponent;
class ANetherCrownEnemy;
enum class ENetherCrownSkillKeyEnum : uint8;

UCLASS()
class NETHERCROWN_API ANetherCrownWeapon : public AActor
{
	GENERATED_BODY()

public:
	ANetherCrownWeapon();

	void DisableEquipSphereCollision() const;

	USkeletalMeshComponent* GetWeaponMeshComponent() const { return WeaponMeshComponent; }

	void SetWeaponHitTraceEnable(const bool bEnableWeaponHitTrace) const;
	void InitWeaponTraceComponentSettings() const;

	void SetTraceMode(const ENetherCrownTraceMode InTraceMode);

	const UNetherCrownWeaponData* GetWeaponData() const { return WeaponData; }

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_ActiveWeaponSkillAuraNiagara(const bool bActive, const ENetherCrownSkillKeyEnum SkillKeyEnum);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_ActiveWeaponLastComboAttackAuraNiagara(const bool bActive);

	void SetActiveWeaponIdleAuraNiagaraComponent(const bool bActive);

	void RiseUpWeapon();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetActiveWeaponIdleAuraNiagaraComponent(const bool bActive);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayRiseUpSound();

	UFUNCTION()
	void HandleOnEquipSphereBeginOverlap(UPrimitiveComponent* OnComponentBeginOverlap, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void HandleOnEquipSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void HandleOnHitEnemy(ANetherCrownEnemy* HitEnemy, const FVector& HitLocation) const;

	void SetEquipComponentSetting(AActor* OtherActor, const bool bCanEquip);

	void ActiveWeaponSkillAuraNiagara(const bool bActive, const ENetherCrownSkillKeyEnum SkillKeyEnum) const;
	void ActiveWeaponLastComboAttackAuraNiagara(const bool bActive) const;

	void CacheWeaponSkillAuraMap();
	void CacheWeaponLastComboAttackAura();

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<USphereComponent> WeaponEquipSphereComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNetherCrownWeaponTraceComponent> WeaponTraceComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FGameplayTag WeaponTag{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNiagaraComponent> WeaponAuraNiagaraComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNiagaraComponent> WeaponIdleAuraNiagaraComponent{};

	UPROPERTY(Transient)
	TMap<ENetherCrownSkillKeyEnum, TObjectPtr<UNiagaraSystem>> CachedWeaponSkillAuraMap{};

	UPROPERTY(Transient)
	TObjectPtr<UNiagaraSystem> CachedWeaponLastComboAttackAura{};

	UPROPERTY(Transient)
	TObjectPtr<UNetherCrownWeaponData> WeaponData{};

	bool bCanRise{ false };

	float WeaponRiseElapsedTime{};

	FVector HiddenLocation{};
	FVector RaisedLocation{};
};
