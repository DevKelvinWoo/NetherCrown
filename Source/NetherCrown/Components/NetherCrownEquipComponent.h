// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrownEquipComponent.generated.h"

class ANetherCrownWeapon;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownEquipComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownEquipComponent();

	void SetCanEquip(const bool bInCanEquip) { bCanEquip = bInCanEquip; }
	void SetEquipableWeapon(ANetherCrownWeapon* InEquipableWeapon);

	void EquipWeapon();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UFUNCTION(Server, Reliable)
	void Server_EquipWeapon();

	bool bCanEquip{ false };

	TWeakObjectPtr<ANetherCrownWeapon> EquipableWeaponWeak{};

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownWeapon> EquipedWeapon{};

	UPROPERTY()
	FName WeaponSocketName{ TEXT("WeaponSocket") };
};
