// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownPlayerController.h"

#include "NetherCrown/NetherCrown.h"
#include "NetherCrownCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ANetherCrownPlayerController::ANetherCrownPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ANetherCrownPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	AddIMCAndBindAction();
}

void ANetherCrownPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	CachedCharacter = Cast<ANetherCrownCharacter>(InPawn);
}

void ANetherCrownPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
	CachedCharacter = nullptr;
}

void ANetherCrownPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);
	CachedCharacter = Cast<ANetherCrownCharacter>(P);
}

void ANetherCrownPlayerController::AddIMCAndBindAction()
{
	UEnhancedInputComponent* EnhancedPlayerInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	check(EnhancedPlayerInputComponent);

	UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(EnhancedInputLocalPlayerSubSystem);

	if (!ensure(MappingContext && MoveAction && LookAtAction && JumpAction && AttackBasicAction && EquipAction && ChangeWeaponAction
					&& QSkillAction && ESkillAction && RSkillAction && ShiftSkillAction))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("Can't add Mapping Context and InputActions in %hs"), __FUNCTION__);

		return;
	}

	//@NOTE : 입력 이벤트는 client에서만 실행된다
	EnhancedInputLocalPlayerSubSystem->AddMappingContext(MappingContext, 0);

	EnhancedPlayerInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::HandleInputMoveCharacter);
	EnhancedPlayerInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ThisClass::HandleOnMoveActionCompleted);
	EnhancedPlayerInputComponent->BindAction(LookAtAction, ETriggerEvent::Triggered, this, &ThisClass::HandleInputLookAtCharacter);
	EnhancedPlayerInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::HandleInputJumpCharacter);
	EnhancedPlayerInputComponent->BindAction(AttackBasicAction, ETriggerEvent::Started, this, &ThisClass::HandleInputRequestBasicAttack);
	EnhancedPlayerInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ThisClass::HandleInputEquipCharacter);
	EnhancedPlayerInputComponent->BindAction(ChangeWeaponAction, ETriggerEvent::Started, this, &ThisClass::HandleInputChangeWeapon);
	EnhancedPlayerInputComponent->BindAction(QSkillAction, ETriggerEvent::Started, this, &ThisClass::HandleInputActiveQSkill);
	EnhancedPlayerInputComponent->BindAction(ESkillAction, ETriggerEvent::Started, this, &ThisClass::HandleInputActiveESkill);
	EnhancedPlayerInputComponent->BindAction(RSkillAction, ETriggerEvent::Started, this, &ThisClass::HandleInputActiveRSkill);
	EnhancedPlayerInputComponent->BindAction(ShiftSkillAction, ETriggerEvent::Started, this, &ThisClass::HandleInputActiveShiftSkill);
}

void ANetherCrownPlayerController::HandleInputMoveCharacter(const FInputActionValue& InActionValue)
{
	ExecuteCharacterAction(&ANetherCrownCharacter::MoveCharacter, InActionValue);
}

void ANetherCrownPlayerController::HandleInputLookAtCharacter(const FInputActionValue& InActionValue)
{
	ExecuteCharacterAction(&ANetherCrownCharacter::LookAtCharacter, InActionValue);
}

void ANetherCrownPlayerController::HandleInputJumpCharacter(const FInputActionValue& InActionValue)
{
	ExecuteCharacterAction(&ANetherCrownCharacter::JumpCharacter, InActionValue);
}

void ANetherCrownPlayerController::HandleOnMoveActionCompleted()
{
	ExecuteCharacterAction(&ANetherCrownCharacter::HandleOnMoveActionCompleted);
}

void ANetherCrownPlayerController::HandleInputRequestBasicAttack(const FInputActionValue& InActionValue)
{
	ExecuteCharacterAction(&ANetherCrownCharacter::RequestBasicAttack, InActionValue);
}

void ANetherCrownPlayerController::HandleInputEquipCharacter(const FInputActionValue& InActionValue)
{
	ExecuteCharacterAction(&ANetherCrownCharacter::EquipCharacter, InActionValue);
}

void ANetherCrownPlayerController::HandleInputChangeWeapon(const FInputActionValue& InActionValue)
{
	ExecuteCharacterAction(&ANetherCrownCharacter::ChangeWeapon, InActionValue);
}

void ANetherCrownPlayerController::HandleInputActiveQSkill(const FInputActionValue& InActionValue)
{
	ExecuteCharacterAction(&ANetherCrownCharacter::ActiveQSkill, InActionValue);
}

void ANetherCrownPlayerController::HandleInputActiveESkill(const FInputActionValue& InActionValue)
{
	ExecuteCharacterAction(&ANetherCrownCharacter::ActiveESkill, InActionValue);
}

void ANetherCrownPlayerController::HandleInputActiveRSkill(const FInputActionValue& InActionValue)
{
	ExecuteCharacterAction(&ANetherCrownCharacter::ActiveRSkill, InActionValue);
}

void ANetherCrownPlayerController::HandleInputActiveShiftSkill(const FInputActionValue& InActionValue)
{
	ExecuteCharacterAction(&ANetherCrownCharacter::ActiveShiftSkill, InActionValue);
}
