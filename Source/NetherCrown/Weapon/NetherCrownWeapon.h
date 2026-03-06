// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "NetherCrownWeapon.generated.h"

class UNiagaraSystem;
class UBoxComponent;
class USphereComponent;
class USkeletalMeshComponent;
class UNiagaraComponent;

class UNetherCrownWeaponData;
class UNetherCrownWeaponTraceComponent;
enum class ENetherCrownSkillKeyEnum : uint8;

USTRUCT()
struct FNetherCrownWeaponTagData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag WeaponTag{};

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag WeaponSwingSound{};
};

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

	const UNetherCrownWeaponData* GetWeaponData() const { return WeaponData; }

	const FNetherCrownWeaponTagData& GetWeaponTagData() const { return WeaponTagData; }

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_ActiveWeaponAuraNiagara(const bool bActive, const ENetherCrownSkillKeyEnum SkillKeyEnum);

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

	void ActiveWeaponAuraNiagara(const bool bActive, const ENetherCrownSkillKeyEnum SkillKeyEnum) const;

	void CacheWeaponAuraMap();

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USphereComponent> WeaponEquipSphereComponent{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNetherCrownWeaponTraceComponent> WeaponTraceComponent{};

	UPROPERTY(Transient)
	TObjectPtr<UNetherCrownWeaponData> WeaponData{};

	UPROPERTY(EditDefaultsOnly)
	FNetherCrownWeaponTagData WeaponTagData{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraComponent> WeaponAuraNiagaraComponent{};

	UPROPERTY(EditDefaultsOnly)
	TMap<ENetherCrownSkillKeyEnum, TSoftObjectPtr<UNiagaraSystem>> WeaponAuraMap{};

	UPROPERTY()
	TMap<ENetherCrownSkillKeyEnum, TObjectPtr<UNiagaraSystem>> CachedWeaponAuraMap{};
};
