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

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleOnEquipSphereBeginOverlap(UPrimitiveComponent* OnComponentBeginOverlap, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void HandleOnEquipSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void HandleOnHitEnemy(AActor* HitEnemy, const FVector& HitLocation) const;

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

	UPROPERTY(Transient)
	TMap<ENetherCrownSkillKeyEnum, TObjectPtr<UNiagaraSystem>> CachedWeaponSkillAuraMap{};

	UPROPERTY(Transient)
	TObjectPtr<UNiagaraSystem> CachedWeaponLastComboAttackAura{};

	UPROPERTY(Transient)
	TObjectPtr<UNetherCrownWeaponData> WeaponData{};
};
