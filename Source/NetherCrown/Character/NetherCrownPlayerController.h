// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownCharacter.h"
#include "NetherCrown/UI/NetherCrownUITypes.h"
#include "GameFramework/PlayerController.h"
#include "NetherCrownPlayerController.generated.h"

class UNetherCrownPrimaryLayout;
class UInputAction;
class UInputMappingContext;

UCLASS()
class NETHERCROWN_API ANetherCrownPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ANetherCrownPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void AcknowledgePossession(APawn* P) override;

private:
	void AddIMCAndBindAction();
	void InitializeUI();

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

	UPROPERTY(EditDefaultsOnly, Category = "InputAction")
	TObjectPtr<UInputAction> MoveAction{};
	UPROPERTY(EditDefaultsOnly, Category = "InputAction")
	TObjectPtr<UInputAction> LookAtAction{};
	UPROPERTY(EditDefaultsOnly, Category = "InputAction")
	TObjectPtr<UInputAction> JumpAction{};
	UPROPERTY(EditDefaultsOnly, Category = "InputAction")
	TObjectPtr<UInputAction> AttackBasicAction{};
	UPROPERTY(EditDefaultsOnly, Category = "InputAction")
	TObjectPtr<UInputAction> EquipAction{};
	UPROPERTY(EditDefaultsOnly, Category = "InputAction")
	TObjectPtr<UInputAction> ChangeWeaponAction{};
	UPROPERTY(EditDefaultsOnly, Category = "InputAction")
	TObjectPtr<UInputAction> QSkillAction{};
	UPROPERTY(EditDefaultsOnly, Category = "InputAction")
	TObjectPtr<UInputAction> ESkillAction{};
	UPROPERTY(EditDefaultsOnly, Category = "InputAction")
	TObjectPtr<UInputAction> RSkillAction{};
	UPROPERTY(EditDefaultsOnly, Category = "InputAction")
	TObjectPtr<UInputAction> ShiftSkillAction{};

	UPROPERTY(EditDefaultsOnly, Category = "InputMappingContext")
	TObjectPtr<UInputMappingContext> MappingContext{};

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownCharacter> CachedCharacter{};
};

template<typename FuncType, typename... ArgsType>
void ANetherCrownPlayerController::ExecuteCharacterAction(FuncType Func, ArgsType&&... Args)
{
	if (!IsLocalController())
	{
		return;
	}

	if (!(IsValid(CachedCharacter)))
	{
		return;
	}

	(CachedCharacter->*Func)(Forward<ArgsType>(Args)...);
}
