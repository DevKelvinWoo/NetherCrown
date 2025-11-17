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

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCameraComponent> MainCameraComponent{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USpringArmComponent> MainSpringArmComponent{};

	void SetCharacterDefaultMovementValues();
};
