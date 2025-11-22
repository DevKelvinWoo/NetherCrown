// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "NetherCrownCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

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

	UFUNCTION(BlueprintCallable)
	bool IsPressedMoveKey() const { return bPressedMoveKey;}

	UFUNCTION(BlueprintCallable)
	bool IsHardLanding() const { return bIsHardLanding; }

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

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCameraComponent> MainCameraComponent{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USpringArmComponent> MainSpringArmComponent{};

	UPROPERTY(Replicated)
	bool bPressedMoveKey{ false };

	UPROPERTY(Replicated)
	bool bIsHardLanding{ false };

	UPROPERTY(Replicated)
	FVector HitPointToGroundWhenJumpStart{};

	FTimerHandle TimerHandle_ResetHardLanding;
};
