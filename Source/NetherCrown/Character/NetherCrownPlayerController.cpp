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

	if (!ensureAlways(MappingContext && MoveAction && LookAtAction))
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't add Mapping Context and InputAction in %hs"), __FUNCTION__);
	}

	EnhancedPlayerInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::MoveCharacter);
	EnhancedPlayerInputComponent->BindAction(LookAtAction, ETriggerEvent::Triggered, this, &ThisClass::LookAtCharacter);
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
