// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "NetherCrownCharacterDefaultSettings.generated.h"

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "NetherCrown Character Settings"))
class NETHERCROWN_API UNetherCrownCharacterDefaultSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	bool IsControllerRotationPitchEnabled() const { return bUseControllerRotationPitch; }
	bool IsControllerRotationYawEnabled() const { return bUseControllerRotationYaw; }
	bool IsControllerRotationRollEnabled() const { return bUseControllerRotationRoll; }
	bool IsMainSpringArmUsePawnControlRotation() const { return bMainSpringArmUsePawnControlRotation; }
	float GetMainSpringArmTargetLength() const { return MainSpringArmTargetLength; }
	const FVector& GetMainSpringArmTargetOffset() const { return MainSpringArmTargetOffset; }
	bool IsMainSpringArmCameraLagEnabled() const { return bMainSpringArmEnableCameraLag; }
	float GetMainSpringArmCameraLagSpeed() const { return MainSpringArmCameraLagSpeed; }
	bool IsMainCameraUsePawnControlRotation() const { return bMainCameraUsePawnControlRotation; }
	const FRotator& GetMainCameraRelativeRotation() const { return MainCameraRelativeRotation; }
	bool IsOrientRotationToMovementEnabled() const { return bOrientRotationToMovement; }
	float GetMaxAcceleration() const { return MaxAcceleration; }
	float GetBrakingDecelerationWalking() const { return BrakingDecelerationWalking; }
	bool IsUseSeparateBrakingFrictionEnabled() const { return bUseSeparateBrakingFriction; }
	float GetMinHardLandingHeight() const { return MinHardLandingHeight; }
	float GetRecoveryResetDelayTime() const { return RecoveryResetDelayTime; }
	const FName& GetEquipWeaponSocketName() const { return EquipWeaponSocketName; }
	const FName& GetStowWeaponSocketLName() const { return StowWeaponSocketLName; }
	const FName& GetStowWeaponSocketRName() const { return StowWeaponSocketRName; }
	const FName& GetWeaponTraceSocketName() const { return WeaponTraceSocketName; }

private:
	UPROPERTY(Config, EditAnywhere)
	bool bUseControllerRotationPitch{ false };

	UPROPERTY(Config, EditAnywhere)
	bool bUseControllerRotationYaw{ false };

	UPROPERTY(Config, EditAnywhere)
	bool bUseControllerRotationRoll{ false };

	UPROPERTY(Config, EditAnywhere)
	bool bMainSpringArmUsePawnControlRotation{ true };

	UPROPERTY(Config, EditAnywhere)
	float MainSpringArmTargetLength{ 535.f };

	UPROPERTY(Config, EditAnywhere)
	FVector MainSpringArmTargetOffset{ 0.f, 0.f, 180.f };

	UPROPERTY(Config, EditAnywhere)
	bool bMainSpringArmEnableCameraLag{ true };

	UPROPERTY(Config, EditAnywhere)
	float MainSpringArmCameraLagSpeed{ 7.f };

	UPROPERTY(Config, EditAnywhere)
	bool bMainCameraUsePawnControlRotation{ false };

	UPROPERTY(Config, EditAnywhere)
	FRotator MainCameraRelativeRotation{ -20.f, 0.f, 0.f };

	UPROPERTY(Config, EditAnywhere)
	bool bOrientRotationToMovement{ true };

	UPROPERTY(Config, EditAnywhere)
	float MaxAcceleration{ 1000.f };

	UPROPERTY(Config, EditAnywhere)
	float BrakingDecelerationWalking{ 1200.f };

	UPROPERTY(Config, EditAnywhere)
	bool bUseSeparateBrakingFriction{ true };

	UPROPERTY(Config, EditAnywhere)
	float MinHardLandingHeight{ 200.f };

	UPROPERTY(Config, EditAnywhere)
	float RecoveryResetDelayTime{ 1.f };

	UPROPERTY(Config, EditAnywhere)
	FName EquipWeaponSocketName{ TEXT("WeaponSocket") };

	UPROPERTY(Config, EditAnywhere)
	FName StowWeaponSocketLName{ TEXT("StowWeaponSocketL") };

	UPROPERTY(Config, EditAnywhere)
	FName StowWeaponSocketRName{ TEXT("StowWeaponSocketR") };

	UPROPERTY(Config, EditAnywhere)
	FName WeaponTraceSocketName{ TEXT("TraceEnd") };
};
