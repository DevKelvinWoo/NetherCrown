// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrown/Data/NetherCrownEquipData.h"
#include "NetherCrownEquipComponent.generated.h"

class UNetherCrownWeaponData;
class UAnimMontage;

class ANetherCrownWeapon;
class ANetherCrownCharacter;

UENUM()
enum class ENetherCrownStowWeaponPosition : uint8
{
	Left,
	Right
};

UENUM()
enum class ENetherCrownEquipState : uint8
{
	Unequipped,
	Equipping,
	Equipped,
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownEquipComponent : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquipWeapon, const bool);

public:
	UNetherCrownEquipComponent();

	bool IsEquipping() const { return EquipState == ENetherCrownEquipState::Equipping; }

	void SetCanEquip(const bool bInCanEquip) { bCanEquip = bInCanEquip; }
	void SetEquipableWeapon(ANetherCrownWeapon* InEquipableWeapon);

	void EquipOrStowWeapon();
	void ChangeWeapon();

	ANetherCrownWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	const UNetherCrownWeaponData* GetEquippedWeaponData() const;

	FOnEquipWeapon& GetOnEquipWeapon() { return OnEquipWeapon; }

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

	void SetupEquipStateTimer();
	void HandleEquipStart();
	void HandleEquipEnd();

	void EquipOrStowWeaponInternal();
	void ChangeWeaponInternal();

	void StowCurrentWeapon();

	void CacheInitData();
	void LoadEquipData();

	bool bCanEquip{ false };

	TWeakObjectPtr<ANetherCrownWeapon> EquipableWeaponWeak{};

	UPROPERTY(Transient, Replicated)
	TObjectPtr<ANetherCrownWeapon> EquippedWeapon{};

	UPROPERTY(Transient)
	FNetherCrownEquipData EquipData{};

	UPROPERTY(EditDefaultsOnly, Category = "EquipDataAsset")
	TSoftObjectPtr<UNetherCrownEquipDataAsset> EquipDataAssetSoft{};

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CachedEquipMontage{};

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownCharacter> CachedCharacter{};

	TArray<TPair<ENetherCrownStowWeaponPosition, ANetherCrownWeapon*>> StowWeaponContainer{};

	FTimerHandle EquipStartTimerHandle{};
	FTimerHandle EquipEndTimerHandle{};

	UPROPERTY(Replicated)
	ENetherCrownEquipState EquipState{ ENetherCrownEquipState::Unequipped };

	FOnEquipWeapon OnEquipWeapon;
};
