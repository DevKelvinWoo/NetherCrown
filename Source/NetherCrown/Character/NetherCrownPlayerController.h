// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NetherCrownPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;

UCLASS()
class NETHERCROWN_API ANetherCrownPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ANetherCrownPlayerController();

protected:
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> MoveAction{};
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> LookAtAction{};
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> JumpAction{};
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> AttackBasicAction{};
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> EquipAction{};
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> ChangeWeaponAction{};
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> QSkillAction{};
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> ESkillAction{};
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> RSkillAction{};
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> ShiftSkillAction{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputMappingContext> MappingContext{};

	void AddIMCAndBindAction();

	UFUNCTION()
	void MoveCharacter(const FInputActionValue& InActionValue);
	UFUNCTION()
	void LookAtCharacter(const FInputActionValue& InActionValue);
	UFUNCTION()
	void JumpCharacter(const FInputActionValue& InActionValue);
	UFUNCTION()
	void HandleOnMoveActionCompleted();
	UFUNCTION()
	void RequestBasicAttack(const FInputActionValue& InActionValue);
	UFUNCTION()
	void EquipCharacter(const FInputActionValue& InActionValue);
	UFUNCTION()
	void ChangeWeapon(const FInputActionValue& InActionValue);
	UFUNCTION()
	void ActiveQSkill(const FInputActionValue& InActionValue);
	UFUNCTION()
	void ActiveESkill(const FInputActionValue& InActionValue);
	UFUNCTION()
	void ActiveRSkill(const FInputActionValue& InActionValue);
	UFUNCTION()
	void ActiveShiftSkill(const FInputActionValue& InActionValue);
};
