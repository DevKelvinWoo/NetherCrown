// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ANetherCrownCharacter::ANetherCrownCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	MainSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("MainSpringArmComponent"));
	MainSpringArmComponent->SetupAttachment(RootComponent);
	MainSpringArmComponent->bUsePawnControlRotation = true;

	MainCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCameraComponent"));
	MainCameraComponent->SetupAttachment(MainSpringArmComponent);
	MainCameraComponent->bUsePawnControlRotation = false;

	SetCharacterDefaultMovementValues();
}

void ANetherCrownCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ANetherCrownCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!(GetCharacterMovement()->IsFalling()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't move character to falling"));
	}
}

void ANetherCrownCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ANetherCrownCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bPressedMoveKey);
}

void ANetherCrownCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	const FVector& HitPointToGroundWhenLanded{ CalculateHitPointToGround() };
	const FVector& DistanceBetweenLandHitPoints{ HitPointToGroundWhenJumpStart - HitPointToGroundWhenLanded };
	const double Distance{ DistanceBetweenLandHitPoints.Length() };

	constexpr double MinHardLandHeight{ 50.0 };
	const bool IsHardLanding{ Distance > MinHardLandHeight };
	Server_SetIsHardLanding(IsHardLanding);
}

void ANetherCrownCharacter::Server_SetPressedMoveKey_Implementation(const bool InbPressedMoveKey)
{
	bPressedMoveKey = InbPressedMoveKey;
}

void ANetherCrownCharacter::Server_SetIsHardLanding_Implementation(const bool InbIsHardLanding)
{
	bIsHardLanding = InbIsHardLanding;
}

void ANetherCrownCharacter::SetCharacterDefaultMovementValues()
{
	UCharacterMovementComponent* MovementComponent{ GetCharacterMovement() };
	check(MovementComponent);

	MovementComponent->bOrientRotationToMovement = true;
}

const FVector ANetherCrownCharacter::CalculateHitPointToGround()
{
	const UWorld* World{ GetWorld() };
	check(World);

	const FVector& LineTraceStartLocation{ GetActorLocation() };
	constexpr float LineTraceLength{ 2000.f };
	const FVector& LineTraceDirection{ GetActorUpVector() * -1.f };
	const FVector& LineTraceEndLocation{ LineTraceStartLocation + LineTraceDirection * LineTraceLength };

	FHitResult HitResult;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	const bool bHit{ GetWorld()->LineTraceSingleByChannel(
	HitResult, LineTraceStartLocation, LineTraceDirection, ECC_Visibility, Params
	)};

	if (bHit)
	{
		return HitResult.ImpactPoint;
	}

	return FVector::ZeroVector;
}

void ANetherCrownCharacter::MoveCharacter(const FInputActionValue& Value)
{
	Server_SetPressedMoveKey(true);

	check(Controller);
	if (Value.IsNonZero())
	{
		const FVector2D& MovementVector{ Value.Get<FVector2D>() };

		const FRotator& Rotation{ Controller->GetControlRotation() };
		const FRotator& YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector& ForwardDirection{ FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) };
		const FVector& RightDirection{ FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) };

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ANetherCrownCharacter::LookAtCharacter(const FInputActionValue& Value)
{
	if (Value.IsNonZero())
	{
		const FVector2D& MovementVector{ Value.Get<FVector2D>() };

		AddControllerYawInput(MovementVector.X);
		AddControllerPitchInput(MovementVector.Y);
	}
}

void ANetherCrownCharacter::JumpCharacter(const FInputActionValue& Value)
{
	HitPointToGroundWhenJumpStart = CalculateHitPointToGround();

	if (Value.IsNonZero())
	{
		const bool bJumpActionInput{ Value.Get<bool>() };
		if (bJumpActionInput)
		{
			Jump();
		}
	}
}

void ANetherCrownCharacter::HandleOnMoveActionCompleted()
{
	Server_SetPressedMoveKey(false);
}
