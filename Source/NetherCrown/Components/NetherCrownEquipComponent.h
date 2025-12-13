// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrownEquipComponent.generated.h"

class UAnimMontage;

class ANetherCrownWeapon;

UENUM()
enum class EStowWeaponPosition : uint8
{
	Left,
	Right
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownEquipComponent : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquipWeapon, const bool);

public:
	UNetherCrownEquipComponent();

	void SetCanEquip(const bool bInCanEquip) { bCanEquip = bInCanEquip; }
	void SetEquipableWeapon(ANetherCrownWeapon* InEquipableWeapon);

	void EquipOrStowWeapon();
	void ChangeWeapon();

	FOnEquipWeapon& GetOnEquipWeapon() { return OnEquipWeapon; }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

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

	bool bCanEquip{ false };

	TWeakObjectPtr<ANetherCrownWeapon> EquipableWeaponWeak{};

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownWeapon> EquipedWeapon{};

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> EquipAnimMontageSoft{};

	TArray<TPair<EStowWeaponPosition, ANetherCrownWeapon*>> StowWeaponContainer{};

	FOnEquipWeapon OnEquipWeapon;
};
