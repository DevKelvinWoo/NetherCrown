// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownPlayerController.h"

#include "NetherCrownCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void ANetherCrownPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	AddIMCAndBindAction();
}

void ANetherCrownPlayerController::AddIMCAndBindAction()
{
	UEnhancedInputComponent* EnhancedPlayerInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	check(EnhancedPlayerInputComponent);

	UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(EnhancedInputLocalPlayerSubSystem);

	ensureAlways(MappingContext);

	EnhancedInputLocalPlayerSubSystem->AddMappingContext(MappingContext, 0);

	if (!ensureAlways(MappingContext && MoveAction && LookAtAction && JumpAction && AttackBasicAction && EquipAction && ChangeWeaponAction
					&& QSkillAction && ESkillAction))
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't add Mapping Context and InputActions in %hs"), __FUNCTION__);

		return;
	}

	EnhancedPlayerInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::MoveCharacter);
	EnhancedPlayerInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ThisClass::HandleOnMoveActionCompleted);
	EnhancedPlayerInputComponent->BindAction(LookAtAction, ETriggerEvent::Triggered, this, &ThisClass::LookAtCharacter);
	EnhancedPlayerInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::JumpCharacter);
	EnhancedPlayerInputComponent->BindAction(AttackBasicAction, ETriggerEvent::Started, this, &ThisClass::RequestBasicAttack);
	EnhancedPlayerInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ThisClass::EquipCharacter);
	EnhancedPlayerInputComponent->BindAction(ChangeWeaponAction, ETriggerEvent::Started, this, &ThisClass::ChangeWeapon);
	EnhancedPlayerInputComponent->BindAction(QSkillAction, ETriggerEvent::Started, this, &ThisClass::ActiveQSkill);
	EnhancedPlayerInputComponent->BindAction(ESkillAction, ETriggerEvent::Started, this, &ThisClass::ActiveESkill);
}

void ANetherCrownPlayerController::MoveCharacter(const FInputActionValue& InActionValue)
{
	ANetherCrownCharacter* NetherCrownCharacter{ Cast<ANetherCrownCharacter>(GetCharacter()) };
	if (!ensureAlways(NetherCrownCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("There is No Possessed Character in %hs"), __FUNCTION__);

		return;
	}

	NetherCrownCharacter->MoveCharacter(InActionValue);
}

void ANetherCrownPlayerController::LookAtCharacter(const FInputActionValue& InActionValue)
{
	ANetherCrownCharacter* NetherCrownCharacter{ Cast<ANetherCrownCharacter>(GetCharacter()) };
	if (!ensureAlways(NetherCrownCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("There is No Possessed Character in %hs"), __FUNCTION__);

		return;
	}

	NetherCrownCharacter->LookAtCharacter(InActionValue);
}

void ANetherCrownPlayerController::JumpCharacter(const FInputActionValue& InActionValue)
{
	ANetherCrownCharacter* NetherCrownCharacter{ Cast<ANetherCrownCharacter>(GetCharacter()) };
	if (!ensureAlways(NetherCrownCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("There is No Possessed Character in %hs"), __FUNCTION__);

		return;
	}

	NetherCrownCharacter->JumpCharacter(InActionValue);
}

void ANetherCrownPlayerController::HandleOnMoveActionCompleted()
{
	ANetherCrownCharacter* NetherCrownCharacter{ Cast<ANetherCrownCharacter>(GetCharacter()) };
	if (!ensureAlways(NetherCrownCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("There is No Possessed Character in %hs"), __FUNCTION__);

		return;
	}

	NetherCrownCharacter->HandleOnMoveActionCompleted();
}

void ANetherCrownPlayerController::RequestBasicAttack(const FInputActionValue& InActionValue)
{
	ANetherCrownCharacter* NetherCrownCharacter{ Cast<ANetherCrownCharacter>(GetCharacter()) };
	if (!ensureAlways(NetherCrownCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("There is No Possessed Character in %hs"), __FUNCTION__);

		return;
	}

	NetherCrownCharacter->RequestBasicAttack(InActionValue);
}

void ANetherCrownPlayerController::EquipCharacter(const FInputActionValue& InActionValue)
{
	ANetherCrownCharacter* NetherCrownCharacter{ Cast<ANetherCrownCharacter>(GetCharacter()) };
	if (!ensureAlways(NetherCrownCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("There is No Possessed Character in %hs"), __FUNCTION__);

		return;
	}

	NetherCrownCharacter->EquipCharacter(InActionValue);
}

void ANetherCrownPlayerController::ChangeWeapon(const FInputActionValue& InActionValue)
{
	ANetherCrownCharacter* NetherCrownCharacter{ Cast<ANetherCrownCharacter>(GetCharacter()) };
	if (!ensureAlways(NetherCrownCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("There is No Possessed Character in %hs"), __FUNCTION__);

		return;
	}

	NetherCrownCharacter->ChangeWeapon(InActionValue);
}

void ANetherCrownPlayerController::ActiveQSkill(const FInputActionValue& InActionValue)
{
	ANetherCrownCharacter* NetherCrownCharacter{ Cast<ANetherCrownCharacter>(GetCharacter()) };
	if (!ensureAlways(NetherCrownCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("There is No Possessed Character in %hs"), __FUNCTION__);

		return;
	}

	NetherCrownCharacter->ActiveQSkill(InActionValue);
}

void ANetherCrownPlayerController::ActiveESkill(const FInputActionValue& InActionValue)
{
	ANetherCrownCharacter* NetherCrownCharacter{ Cast<ANetherCrownCharacter>(GetCharacter()) };
	if (!ensureAlways(NetherCrownCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("There is No Possessed Character in %hs"), __FUNCTION__);

		return;
	}

	NetherCrownCharacter->ActiveESkill(InActionValue);
}
