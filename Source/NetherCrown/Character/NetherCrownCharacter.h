// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "NetherCrownCharacter.generated.h"

class UNetherCrownCrowdControlComponent;
class USpringArmComponent;
class UCameraComponent;

class UNetherCrownBasicAttackComponent;
class UNetherCrownEquipComponent;
class UNetherCrownSkillComponent;

USTRUCT()
struct FNetherCrownCharacterTagData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag JumpStartSoundTag{};

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag HardLandingSoundTag{};
};

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
	void ActiveQSkill(const FInputActionValue& Value);
	void ActiveESkill(const FInputActionValue& Value);
	void ActiveRSkill(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	bool IsPressedMoveKey() const { return bPressedMoveKey;}

	UFUNCTION(BlueprintCallable)
	bool IsHardLanding() const { return bIsHardLanding; }

	UNetherCrownBasicAttackComponent* GetBasicAttackComponent() const { return NetherCrownBasicAttackComponent; }
	UNetherCrownEquipComponent* GetEquipComponent() const { return NetherCrownEquipComponent; }
	UNetherCrownSkillComponent* GetSkillComponent() const { return NetherCrownSkillComponent; }

	void SetSpringArmZOffset(float InSpringArmZOffset) const;
	void SetSpringArmLength(float InSpringArmLength) const;

protected:
	virtual void BeginPlay() override;
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

	//@NOTE : 만약 공격도 막아야 한다면 두 가지 버전의 함수를 오버로딩하자 (공격 + 움직임 제한, 움직임만 제한)
	void SetEnableCharacterControl(const bool bEnableMovement) const;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCameraComponent> MainCameraComponent{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USpringArmComponent> MainSpringArmComponent{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNetherCrownBasicAttackComponent> NetherCrownBasicAttackComponent{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNetherCrownEquipComponent> NetherCrownEquipComponent{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNetherCrownSkillComponent> NetherCrownSkillComponent{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNetherCrownCrowdControlComponent> NetherCrownCrowdControlComponent{};

	UPROPERTY(Replicated)
	bool bPressedMoveKey{ false };

	UPROPERTY(Replicated)
	bool bIsHardLanding{ false };

	UPROPERTY(Replicated)
	FVector HitPointToGroundWhenJumpStart{};

	UPROPERTY(EditDefaultsOnly)
	FNetherCrownCharacterTagData CharacterTagData{};

	FTimerHandle TimerHandle_ResetHardLanding;
};
