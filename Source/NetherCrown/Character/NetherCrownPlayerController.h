// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownCharacter.h"
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
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void AcknowledgePossession(APawn* P) override;

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
	void HandleInputMoveCharacter(const FInputActionValue& InActionValue);
	UFUNCTION()
	void HandleInputLookAtCharacter(const FInputActionValue& InActionValue);
	UFUNCTION()
	void HandleInputJumpCharacter(const FInputActionValue& InActionValue);
	UFUNCTION()
	void HandleOnMoveActionCompleted();
	UFUNCTION()
	void HandleInputRequestBasicAttack(const FInputActionValue& InActionValue);
	UFUNCTION()
	void HandleInputEquipCharacter(const FInputActionValue& InActionValue);
	UFUNCTION()
	void HandleInputChangeWeapon(const FInputActionValue& InActionValue);
	UFUNCTION()
	void HandleInputActiveQSkill(const FInputActionValue& InActionValue);
	UFUNCTION()
	void HandleInputActiveESkill(const FInputActionValue& InActionValue);
	UFUNCTION()
	void HandleInputActiveRSkill(const FInputActionValue& InActionValue);
	UFUNCTION()
	void HandleInputActiveShiftSkill(const FInputActionValue& InActionValue);

	template<typename FuncType, typename... ArgsType>
	void ExecuteCharacterAction(FuncType Func, ArgsType&&... Args);

	UPROPERTY()
	TObjectPtr<ANetherCrownCharacter> CachedCharacter{};
};

template<typename FuncType, typename... ArgsType>
void ANetherCrownPlayerController::ExecuteCharacterAction(FuncType Func, ArgsType&&... Args)
{
	if (!IsLocalController())
	{
		return;
	}

	if (!ensureAlways(IsValid(CachedCharacter)))
	{
		return;
	}

	(CachedCharacter->*Func)(Forward<ArgsType>(Args)...);
}
