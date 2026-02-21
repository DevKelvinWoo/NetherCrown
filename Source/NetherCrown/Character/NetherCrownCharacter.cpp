// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownCharacter.h"

#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/PostProcessComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

#include "NetherCrown/Components/NetherCrownBasicAttackComponent.h"
#include "NetherCrown/Components/NetherCrownControlGhostTrailComponent.h"
#include "NetherCrown/Components/NetherCrownControlPPComponent.h"
#include "NetherCrown/Components/NetherCrownCrowdControlComponent.h"
#include "NetherCrown/Components/NetherCrownEquipComponent.h"
#include "NetherCrown/Components/NetherCrownSkillComponent.h"
#include "NetherCrown/Settings/NetherCrownCharacterDefaultSettings.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

ANetherCrownCharacter::ANetherCrownCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SetUseControllerSettings();

	MainSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("MainSpringArmComponent"));
	MainSpringArmComponent->SetupAttachment(RootComponent);
	SetMainSpringArmComponentSettings();

	MainCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCameraComponent"));
	MainCameraComponent->SetupAttachment(MainSpringArmComponent);
	SetMainCameraComponentSettings();

	SetCharacterDefaultMovementSettings();

	NetherCrownGhostTrailNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("GhostTrailNiagaraComponent"));
	NetherCrownGhostTrailNiagaraComponent->SetupAttachment(RootComponent);

	NetherCrownBasicAttackComponent = CreateDefaultSubobject<UNetherCrownBasicAttackComponent>(TEXT("BasicAttackComponent"));
	NetherCrownEquipComponent = CreateDefaultSubobject<UNetherCrownEquipComponent>(TEXT("EquipComponent"));
	NetherCrownSkillComponent = CreateDefaultSubobject<UNetherCrownSkillComponent>(TEXT("SkillComponent"));
	NetherCrownCrowdControlComponent = CreateDefaultSubobject<UNetherCrownCrowdControlComponent>(TEXT("CrowdControlComponent"));
	NetherCrownPostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
	NetherCrownControlPPComponent = CreateDefaultSubobject<UNetherCrownControlPPComponent>(TEXT("ControlPPComponent"));
	NetherCrownControlGhostTrailComponent = CreateDefaultSubobject<UNetherCrownControlGhostTrailComponent>(TEXT("ControlGhostTrailComponent"));
}

void ANetherCrownCharacter::SetUseControllerSettings()
{
	const UNetherCrownCharacterDefaultSettings* CharacterDefaultSetting{ GetDefault<UNetherCrownCharacterDefaultSettings>() };
	check(CharacterDefaultSetting);

	bUseControllerRotationPitch = CharacterDefaultSetting->IsControllerRotationPitchEnabled();
	bUseControllerRotationYaw = CharacterDefaultSetting->IsControllerRotationYawEnabled();
	bUseControllerRotationRoll = CharacterDefaultSetting->IsControllerRotationRollEnabled();
}

void ANetherCrownCharacter::SetMainSpringArmComponentSettings()
{
	const UNetherCrownCharacterDefaultSettings* CharacterDefaultSetting{ GetDefault<UNetherCrownCharacterDefaultSettings>() };
	check(CharacterDefaultSetting);

	check(MainSpringArmComponent);
	MainSpringArmComponent->bUsePawnControlRotation = CharacterDefaultSetting->IsMainSpringArmUsePawnControlRotation();
	MainSpringArmComponent->TargetArmLength = CharacterDefaultSetting->GetMainSpringArmTargetLength();
	MainSpringArmComponent->TargetOffset = CharacterDefaultSetting->GetMainSpringArmTargetOffset();
	MainSpringArmComponent->bEnableCameraLag = CharacterDefaultSetting->IsMainSpringArmCameraLagEnabled();
	MainSpringArmComponent->CameraLagSpeed = CharacterDefaultSetting->GetMainSpringArmCameraLagSpeed();
}

void ANetherCrownCharacter::SetMainCameraComponentSettings()
{
	const UNetherCrownCharacterDefaultSettings* CharacterDefaultSetting{ GetDefault<UNetherCrownCharacterDefaultSettings>() };
	check(CharacterDefaultSetting);

	check(MainCameraComponent);
	MainCameraComponent->bUsePawnControlRotation = CharacterDefaultSetting->IsMainCameraUsePawnControlRotation();
	MainCameraComponent->SetRelativeRotation(CharacterDefaultSetting->GetMainCameraRelativeRotation());
}

void ANetherCrownCharacter::SetCharacterDefaultMovementSettings()
{
	UCharacterMovementComponent* MovementComponent{ GetCharacterMovement() };
	check(MovementComponent);

	const UNetherCrownCharacterDefaultSettings* CharacterDefaultSetting{ GetDefault<UNetherCrownCharacterDefaultSettings>() };
	check(CharacterDefaultSetting);

	MovementComponent->bOrientRotationToMovement = CharacterDefaultSetting->IsOrientRotationToMovementEnabled();
	MovementComponent->MaxAcceleration = CharacterDefaultSetting->GetMaxAcceleration();
	MovementComponent->BrakingDecelerationWalking = CharacterDefaultSetting->GetBrakingDecelerationWalking();
	MovementComponent->bUseSeparateBrakingFriction = CharacterDefaultSetting->IsUseSeparateBrakingFrictionEnabled();
}

void ANetherCrownCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(NetherCrownBasicAttackComponent);
	NetherCrownBasicAttackComponent->GetOnStopOrStartBasicAttack().AddUObject(this, &ThisClass::SetCharacterMovementControl);

	check(NetherCrownEquipComponent);
	NetherCrownEquipComponent->GetOnEquipEndOrStart().AddUObject(this, &ThisClass::SetCharacterMovementControl);

	check(NetherCrownSkillComponent);
	NetherCrownSkillComponent->GetOnStopOrStartSkill().AddUObject(this, &ThisClass::SetCharacterMovementControl);

	check(NetherCrownControlPPComponent);
	NetherCrownControlPPComponent->SetHandlingPostProcessComponent(NetherCrownPostProcessComponent);

	check(NetherCrownControlGhostTrailComponent);
	NetherCrownControlGhostTrailComponent->SetHandledGhostTrailNiagaraComponent(NetherCrownGhostTrailNiagaraComponent);
}

void ANetherCrownCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bPressedMoveKey);
	DOREPLIFETIME(ThisClass, bIsHardLanding);
	DOREPLIFETIME(ThisClass, JumpStartLocation);
}

void ANetherCrownCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	//@NOTE : bIsHardLanding이 replicate되기 전에 AnimState가 돌아서 Server와 Client 둘 다에서 true로 만들어 놓음, 그리고 n초뒤에 다시 false로 변경함
	const bool bShouldHandleLocally = HasAuthority() || IsLocallyControlled();
	if (bShouldHandleLocally)
	{
		check(NetherCrownBasicAttackComponent);
		NetherCrownBasicAttackComponent->SetCanAttack(true);

		CheckIsHardLandingAndSetTimer();
		DisableMovementWhenHardLanding();

		if (!HasAuthority() && bIsHardLanding)
		{
			FNetherCrownUtilManager::PlaySound2DByGameplayTag(this, CharacterTagData.HardLandingSoundTag);
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
		JumpStartLocation = GetActorLocation();
	}
	else
	{
		FNetherCrownUtilManager::PlaySound2DByGameplayTag(this, CharacterTagData.JumpStartSoundTag);
	}
}

void ANetherCrownCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (HasAuthority() && GetCharacterMovement()->IsFalling())
	{
		bIsHardLanding = false;
		JumpStartLocation = GetActorLocation();
	}
}

void ANetherCrownCharacter::MoveCharacter(const FInputActionValue& Value)
{
	check(Controller);

	Server_SetPressedMoveKey(true);

	//@NOTE : CMC의 클라이언트 예측 로직으로 input lag을 방지한다, 또한 FInputActionValue는 NetSerialize용으로 설계되지 않음 (이는 LookAt, Jump에도 해당함)
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

void ANetherCrownCharacter::Server_SetPressedMoveKey_Implementation(const bool InbPressedMoveKey)
{
	bPressedMoveKey = InbPressedMoveKey;
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
		if (Value.Get<bool>())
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
		if (Value.Get<bool>())
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
		if (Value.Get<bool>())
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
		if (Value.Get<bool>())
		{
			check(NetherCrownEquipComponent);
			NetherCrownEquipComponent->ChangeWeapon();
		}
	}
}

void ANetherCrownCharacter::ExecuteSkillByKey(const FInputActionValue& Value, ENetherCrownSkillKeyEnum SkillKey) const
{
	if (Value.Get<bool>())
	{
		check(NetherCrownSkillComponent);
		NetherCrownSkillComponent->ActivateSkill(SkillKey);
	}
}

void ANetherCrownCharacter::ActiveQSkill(const FInputActionValue& Value) const
{
	ExecuteSkillByKey(Value, ENetherCrownSkillKeyEnum::QSkill);
}

void ANetherCrownCharacter::ActiveESkill(const FInputActionValue& Value) const
{
	ExecuteSkillByKey(Value, ENetherCrownSkillKeyEnum::ESkill);
}

void ANetherCrownCharacter::ActiveRSkill(const FInputActionValue& Value) const
{
	ExecuteSkillByKey(Value, ENetherCrownSkillKeyEnum::RSkill);
}

void ANetherCrownCharacter::ActiveShiftSkill(const FInputActionValue& Value) const
{
	ExecuteSkillByKey(Value, ENetherCrownSkillKeyEnum::ShiftSkill);
}

void ANetherCrownCharacter::SetMainSpringArmZOffset(const float InSpringArmZOffset)
{
	check(MainSpringArmComponent);
	MainSpringArmComponent->TargetOffset.Z = InSpringArmZOffset;
}

void ANetherCrownCharacter::SetMainSpringArmLength(const float InSpringArmLength)
{
	check(MainSpringArmComponent);
	MainSpringArmComponent->TargetArmLength = InSpringArmLength;
}

bool ANetherCrownCharacter::IsEquippedWeapon() const
{
	check(NetherCrownEquipComponent);
	return NetherCrownEquipComponent->GetEquippedWeapon() ? true : false;
}

UNetherCrownStatusEffectControlComponent* ANetherCrownCharacter::GetStatusEffectControlComponent() const
{
	//@TODO : Need to implements Status Effect Control Component in NetherCrownCharacter class (now only for enemy class)
	return nullptr;
}

void ANetherCrownCharacter::CheckIsHardLandingAndSetTimer()
{
	const UNetherCrownCharacterDefaultSettings* CharacterDefaultSetting{ GetDefault<UNetherCrownCharacterDefaultSettings>() };
	check(CharacterDefaultSetting);

	const double DistanceBetweenLandAndJumpStartLocation{ JumpStartLocation.Z - GetActorLocation().Z };
	const double MinHardLandHeight{ CharacterDefaultSetting->GetMinHardLandingHeight() };
	bIsHardLanding = DistanceBetweenLandAndJumpStartLocation > MinHardLandHeight;

	if (bIsHardLanding)
	{
		//@NOTE  : bIsHardLanding이 replicate되기 전에 AnimState가 돌아서 Client단에서 true로 만들어 놓음, 그리고 n초뒤에 다시 false로 변경함
		const float ResetDelay = CharacterDefaultSetting->GetRecoveryResetDelayTime();
		GetWorldTimerManager().ClearTimer(TimerHandle_ResetHardLanding);
		GetWorldTimerManager().SetTimer(TimerHandle_ResetHardLanding, this, &ANetherCrownCharacter::ResetHardLandingState, ResetDelay, false);
	}
}

void ANetherCrownCharacter::ResetHardLandingState()
{
	bIsHardLanding = false;

	UCharacterMovementComponent* MovementComponent{ GetCharacterMovement() };
	check(MovementComponent);

	MovementComponent->SetMovementMode(MOVE_Walking);
}

void ANetherCrownCharacter::DisableMovementWhenHardLanding() const
{
	check(Controller);

	if (bIsHardLanding)
	{
		UCharacterMovementComponent* MovementComponent{ GetCharacterMovement() };
		check(MovementComponent);

		MovementComponent->DisableMovement();
	}
}

void ANetherCrownCharacter::SetCharacterMovementControl(const bool bEnableMovement) const
{
	UCharacterMovementComponent* MovementComponent{ GetCharacterMovement() };
	check(MovementComponent);

	bEnableMovement ? MovementComponent->SetMovementMode(MOVE_Walking) : MovementComponent->DisableMovement();
}
