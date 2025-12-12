// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "NetherCrownCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

class UNetherCrownBasicAttackComponent;
class UNetherCrownEquipComponent;

UCLASS()
class NETHERCROWN_API ANetherCrownCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ANetherCrownCharacter();

	void MoveCharacter(const FInputActionValue& Value);
	void LookAtCharacter(const FInputActionValue& Value);
	void JumpCharacter(const FInputActionValue& Value);
	void HandleOnMoveActionCompleted();
	void RequestBasicAttack(const FInputActionValue& Value);
	void EquipCharacter(const FInputActionValue& Value);
	void ChangeWeapon(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	bool IsPressedMoveKey() const { return bPressedMoveKey;}

	UFUNCTION(BlueprintCallable)
	bool IsHardLanding() const { return bIsHardLanding; }

	UNetherCrownBasicAttackComponent* GetBasicAttackComponent() const { return NetherCrownBasicAttackComponent; }
	UNetherCrownEquipComponent* GetEquipComponent() const { return NetherCrownEquipComponent; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void OnJumped_Implementation() override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

private:
	UFUNCTION(Server, Unreliable)
	void Server_SetPressedMoveKey(const bool InbPressedMoveKey);

	void SetCharacterDefaultMovementValues() const;

	void ResetHardLandingState();

	void CheckIsHardLandingAndSetTimer();

	void BlockInputWhenHardLanding() const;

	void HandleOnStartOrStopBasicAttack(const bool bIsAnimPlay);

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCameraComponent> MainCameraComponent{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USpringArmComponent> MainSpringArmComponent{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNetherCrownBasicAttackComponent> NetherCrownBasicAttackComponent{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNetherCrownEquipComponent> NetherCrownEquipComponent{};

	UPROPERTY(Replicated)
	bool bPressedMoveKey{ false };

	UPROPERTY(Replicated)
	bool bIsHardLanding{ false };

	UPROPERTY(Replicated)
	FVector HitPointToGroundWhenJumpStart{};

	FTimerHandle TimerHandle_ResetHardLanding;
};
