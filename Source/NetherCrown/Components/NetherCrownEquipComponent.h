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
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquipEndOrStart, const bool)

public:
	UNetherCrownEquipComponent();

	void SetCanEquip(const bool bInCanEquip) { bCanEquip = bInCanEquip; }
	void SetEquipableWeapon(ANetherCrownWeapon* InEquipableWeapon);

	void EquipOrStowWeapon();
	void ChangeWeapon();

	void NotifyEquipEndOrStart(const bool bEquipEnd) const;

	const ANetherCrownWeapon* GetEquippedWeapon() { return EquippedWeapon; }

	FOnEquipWeapon& GetOnEquipWeapon() { return OnEquipWeapon; }
	FOnEquipEndOrStart& GetOnEquipEndOrStart() { return OnEquipEndOrStart; }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

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

	UPROPERTY(Transient, Replicated)
	TObjectPtr<ANetherCrownWeapon> EquippedWeapon{};

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> EquipAnimMontageSoft{};

	TArray<TPair<EStowWeaponPosition, ANetherCrownWeapon*>> StowWeaponContainer{};

	FOnEquipWeapon OnEquipWeapon;
	FOnEquipEndOrStart OnEquipEndOrStart;
};
