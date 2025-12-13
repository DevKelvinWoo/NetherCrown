// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

#include "NetherCrown/Components/NetherCrownBasicAttackComponent.h"
#include "NetherCrown/Components/NetherCrownEquipComponent.h"
#include "NetherCrown/Settings/NetherCrownCharacterDefaultSettings.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

ANetherCrownCharacter::ANetherCrownCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	MainSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("MainSpringArmComponent"));
	MainSpringArmComponent->SetupAttachment(RootComponent);
	MainSpringArmComponent->bUsePawnControlRotation = true;
	MainSpringArmComponent->TargetArmLength = 435.f;
	MainSpringArmComponent->TargetOffset = FVector(0.f, 0.f, 200.f);
	MainSpringArmComponent->bEnableCameraLag = true;
	MainSpringArmComponent->CameraLagSpeed = 7.f;

	MainCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCameraComponent"));
	MainCameraComponent->SetupAttachment(MainSpringArmComponent);
	MainCameraComponent->bUsePawnControlRotation = false;
	MainCameraComponent->SetRelativeRotation(FRotator(-25.f, 0.f, 0.f));

	SetCharacterDefaultMovementValues();

	NetherCrownBasicAttackComponent = CreateDefaultSubobject<UNetherCrownBasicAttackComponent>(TEXT("BasicAttackComponent"));
	NetherCrownEquipComponent = CreateDefaultSubobject<UNetherCrownEquipComponent>(TEXT("EquipComponent"));
}

void ANetherCrownCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(NetherCrownBasicAttackComponent);
	NetherCrownBasicAttackComponent->GetOnStopOrStartBasicAttack().AddUObject(this, &ThisClass::SetEnableCharacterControl);

	check(NetherCrownEquipComponent);
	NetherCrownEquipComponent->GetOnEquipEndOrStart().AddUObject(this, &ThisClass::SetEnableCharacterControl);
}

void ANetherCrownCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANetherCrownCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ANetherCrownCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bPressedMoveKey);
	DOREPLIFETIME(ThisClass, bIsHardLanding);
	DOREPLIFETIME(ThisClass, HitPointToGroundWhenJumpStart);
}

void ANetherCrownCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	//@NOTE  : bIsHardLanding이 replicate되기 전에 AnimState가 돌아서 Server와 Client 둘 다에서 true로 만들어 놓음, 그리고 n초뒤에 다시 false로 변경함
	const bool bShouldHandleLocally = HasAuthority() || IsLocallyControlled();
	if (bShouldHandleLocally)
	{
		check(NetherCrownBasicAttackComponent);
		NetherCrownBasicAttackComponent->SetCanAttack(true);

		CheckIsHardLandingAndSetTimer();
		BlockInputWhenHardLanding();

		if (!HasAuthority() && bIsHardLanding)
		{
			FNetherCrownUtilManager::PlaySound2DByGameplayTag(this, NetherCrownTags::Sound_Character_HardLand);
		}
	}
}

void ANetherCrownCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();

	if (HasAuthority())
	{
		check(NetherCrownBasicAttackComponent);
		NetherCrownBasicAttackComponent->SetCanAttack(false);

		bIsHardLanding = false;
		HitPointToGroundWhenJumpStart = GetActorLocation();
	}
	else
	{
		FNetherCrownUtilManager::PlaySound2DByGameplayTag(this, NetherCrownTags::Sound_Character_JumpStart);
	}
}

void ANetherCrownCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (HasAuthority() && GetCharacterMovement()->IsFalling())
	{
		bIsHardLanding = false;
		HitPointToGroundWhenJumpStart = GetActorLocation();
	}
}

void ANetherCrownCharacter::Server_SetPressedMoveKey_Implementation(const bool InbPressedMoveKey)
{
	bPressedMoveKey = InbPressedMoveKey;
}

void ANetherCrownCharacter::SetCharacterDefaultMovementValues() const
{
	UCharacterMovementComponent* MovementComponent{ GetCharacterMovement() };
	check(MovementComponent);

	MovementComponent->bOrientRotationToMovement = true;
	MovementComponent->MaxAcceleration = 1000.f;
	MovementComponent->BrakingDecelerationWalking = 1200.f;
	MovementComponent->bUseSeparateBrakingFriction = true;
}

void ANetherCrownCharacter::ResetHardLandingState()
{
	bIsHardLanding = false;

	UCharacterMovementComponent* MovementComponent{ GetCharacterMovement() };
	check(MovementComponent);

	MovementComponent->SetMovementMode(MOVE_Walking);
}

void ANetherCrownCharacter::CheckIsHardLandingAndSetTimer()
{
	const UNetherCrownCharacterDefaultSettings* CharacterDefaultSetting{ GetDefault<UNetherCrownCharacterDefaultSettings>() };
	check(CharacterDefaultSetting);

	const double DistanceBetweenLandHitPoints{ HitPointToGroundWhenJumpStart.Z - GetActorLocation().Z };

	const double MinHardLandHeight{ CharacterDefaultSetting->MinHardLandingHeight };
	bIsHardLanding = DistanceBetweenLandHitPoints > MinHardLandHeight;

	//@NOTE  : bIsHardLanding이 replicate되기 전에 AnimState가 돌아서 Client단에서 true로 만들어 놓음, 그리고 n초뒤에 다시 false로 변경함
	const float ResetDelay = CharacterDefaultSetting->RecoveryResetDelayTime;
	GetWorldTimerManager().ClearTimer(TimerHandle_ResetHardLanding);
	GetWorldTimerManager().SetTimer(TimerHandle_ResetHardLanding, this, &ANetherCrownCharacter::ResetHardLandingState, ResetDelay, false);
}

void ANetherCrownCharacter::BlockInputWhenHardLanding() const
{
	check(Controller);

	if (bIsHardLanding)
	{
		UCharacterMovementComponent* MovementComponent{ GetCharacterMovement() };
		check(MovementComponent);

		MovementComponent->DisableMovement();
	}
}

void ANetherCrownCharacter::SetEnableCharacterControl(const bool bEnableMovement) const
{
	UCharacterMovementComponent* MovementComponent{ GetCharacterMovement() };
	check(MovementComponent);

	bEnableMovement ? MovementComponent->SetMovementMode(MOVE_Walking) : MovementComponent->DisableMovement();
}

void ANetherCrownCharacter::MoveCharacter(const FInputActionValue& Value)
{
	check(Controller);

	Server_SetPressedMoveKey(true);

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

void ANetherCrownCharacter::RequestBasicAttack(const FInputActionValue& Value)
{
	if (Value.IsNonZero())
	{
		const bool bBasicAttackActionInput{ Value.Get<bool>() };
		if (bBasicAttackActionInput)
		{
			check(NetherCrownBasicAttackComponent);
			NetherCrownBasicAttackComponent->RequestBasicAttack();
		}
	}
}

void ANetherCrownCharacter::EquipCharacter(const FInputActionValue& Value)
{
	if (Value.IsNonZero())
	{
		const bool bEquipKeyInput{ Value.Get<bool>() };
		if (bEquipKeyInput)
		{
			check(NetherCrownEquipComponent);
			NetherCrownEquipComponent->EquipOrStowWeapon();
		}
	}
}

void ANetherCrownCharacter::ChangeWeapon(const FInputActionValue& Value)
{
	if (Value.IsNonZero())
	{
		const bool bEquipKeyInput{ Value.Get<bool>() };
		if (bEquipKeyInput)
		{
			check(NetherCrownEquipComponent);
			NetherCrownEquipComponent->ChangeWeapon();
		}
	}
}
