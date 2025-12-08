// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetherCrownWeapon.generated.h"

class UBoxComponent;
class USphereComponent;
class USkeletalMeshComponent;

UCLASS()
class NETHERCROWN_API ANetherCrownWeapon : public AActor
{
	GENERATED_BODY()

public:
	ANetherCrownWeapon();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void HandleOnEquipSphereBeginOverlap(UPrimitiveComponent* OnComponentBeginOverlap, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void HandleOnEquipSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetEquipComponentSetting(AActor* OtherActor, const bool bCanEquip);

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> WeaponHitBoxComponent{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USphereComponent> WeaponEquipSphereComponent{};
};
