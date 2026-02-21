// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "NetherCrownEquipComponent.generated.h"

class UNetherCrownWeaponData;
class UAnimMontage;

class ANetherCrownWeapon;
class ANetherCrownCharacter;
struct FNetherCrownWeaponTagData;

UENUM()
enum class EStowWeaponPosition : uint8
{
	Left,
	Right
};

USTRUCT()
struct FNetherCrownEquipComponentTagData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag EquipSoundTag{};
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownEquipComponent : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquipWeapon, const bool);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquipEndOrStart, const bool)

public:
	UNetherCrownEquipComponent();

	void SetCanEquip(const bool bInCanEquip) { bCanEquip = bInCanEquip; }
	void SetEquipableWeapon(ANetherCrownWeapon* InEquipableWeapon);

	void EquipOrStowWeapon();
	void ChangeWeapon();

	void NotifyEquipEndOrStart(const bool bEquipEnd) const;

	ANetherCrownWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	const UNetherCrownWeaponData* GetEquippedWeaponData() const;

	const FNetherCrownWeaponTagData& GetEquippedWeaponTagData() const;

	FOnEquipWeapon& GetOnEquipWeapon() { return OnEquipWeapon; }
	FOnEquipEndOrStart& GetOnEquipEndOrStart() { return OnEquipEndOrStart; }

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

private:
	UFUNCTION(Server, Reliable)
	void Server_EquipOrStowWeapon();

	UFUNCTION(Server, Reliable)
	void Server_ChangeWeapon();

	UFUNCTION(NetMulticast, Reliable) //Unreliable 시 패킷 손실로 애니메이션 출력이 안됨
	void Multicast_PlayEquipAnimationAndSound();

	void AttachWeaponToCharacterMesh(ANetherCrownWeapon* TargetWeapon, const FName& WeaponSocketName) const;

	void EquipOrStowWeaponInternal();
	void ChangeWeaponInternal();

	void StowCurrentWeapon();

	void CacheInitData();

	bool bCanEquip{ false };

	TWeakObjectPtr<ANetherCrownWeapon> EquipableWeaponWeak{};

	UPROPERTY(Transient, Replicated)
	TObjectPtr<ANetherCrownWeapon> EquippedWeapon{};

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> EquipAnimMontageSoft{};

	UPROPERTY(EditDefaultsOnly)
	FNetherCrownEquipComponentTagData EquipComponentTagData{};

	UPROPERTY()
	TObjectPtr<UAnimMontage> CachedEquipMontage;

	UPROPERTY()
	TObjectPtr<ANetherCrownCharacter> CachedCharacter{};

	TArray<TPair<EStowWeaponPosition, ANetherCrownWeapon*>> StowWeaponContainer{};

	FOnEquipWeapon OnEquipWeapon;
	FOnEquipEndOrStart OnEquipEndOrStart;
};
