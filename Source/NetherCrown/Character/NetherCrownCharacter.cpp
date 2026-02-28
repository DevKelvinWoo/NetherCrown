// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownCharacter.h"

#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/PostProcessComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/NetherCrown.h"

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

	NetherCrownBasicAttackComponent = CreateDefaultSubobject<UNetherCrownBasicAttackComponent>(TEXT("BasicAttackComponent"));
	NetherCrownEquipComponent = CreateDefaultSubobject<UNetherCrownEquipComponent>(TEXT("EquipComponent"));
	NetherCrownSkillComponent = CreateDefaultSubobject<UNetherCrownSkillComponent>(TEXT("SkillComponent"));
	NetherCrownCrowdControlComponent = CreateDefaultSubobject<UNetherCrownCrowdControlComponent>(TEXT("CrowdControlComponent"));

	SetCharacterDefaultMovementSettings();

	//@NOTE : Only for Client (visual only components), PIE is not checked by IsRunningDedicatedServer
	if (!IsRunningDedicatedServer())
	{
		MainSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("MainSpringArmComponent"));
		MainSpringArmComponent->SetupAttachment(RootComponent);
		SetMainSpringArmComponentSettings();

		MainCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCameraComponent"));
		MainCameraComponent->SetupAttachment(MainSpringArmComponent);
		SetMainCameraComponentSettings();

		NetherCrownGhostTrailNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("GhostTrailNiagaraComponent"));
		NetherCrownGhostTrailNiagaraComponent->SetupAttachment(RootComponent);

		NetherCrownControlGhostTrailComponent = CreateDefaultSubobject<UNetherCrownControlGhostTrailComponent>(TEXT("ControlGhostTrailComponent"));

		NetherCrownPostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
		NetherCrownControlPPComponent = CreateDefaultSubobject<UNetherCrownControlPPComponent>(TEXT("ControlPPComponent"));
	}
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

	DestroyVisualOnlyComponentsOnDS();

	check(NetherCrownBasicAttackComponent);
	NetherCrownBasicAttackComponent->GetOnStopOrStartBasicAttack().AddUObject(this, &ThisClass::SetCharacterMovementControl);

	check(NetherCrownEquipComponent);
	NetherCrownEquipComponent->GetOnEquipEndOrStart().AddUObject(this, &ThisClass::SetCharacterMovementControl);

	check(NetherCrownSkillComponent);
	NetherCrownSkillComponent->GetOnStopOrStartSkill().AddUObject(this, &ThisClass::SetCharacterMovementControl);

	if (IsValid(NetherCrownControlPPComponent) && IsValid(NetherCrownPostProcessComponent))
	{
		NetherCrownControlPPComponent->SetHandlingPostProcessComponent(NetherCrownPostProcessComponent);
	}

	if (IsValid(NetherCrownControlGhostTrailComponent) && IsValid(NetherCrownGhostTrailNiagaraComponent))
	{
		NetherCrownControlGhostTrailComponent->SetHandledGhostTrailNiagaraComponent(NetherCrownGhostTrailNiagaraComponent);
	}
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

	if (HasAuthority())
	{
		check(NetherCrownBasicAttackComponent)
		NetherCrownBasicAttackComponent->SetCanAttack(true);

		SetIsHardLanding();
		DisableMovementAndSetResetTimerWhenHardLanding();
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
	else if (IsLocallyControlled())
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

void ANetherCrownCharacter::DestroyVisualOnlyComponentsOnDS()
{
	if (!IsRunningDedicatedServer())
	{
		return;
	}

	auto SafeDestroyComponent = [](UActorComponent* Component)
		{
			if (IsValid(Component))
			{
				Component->DestroyComponent();
				Component = nullptr;
			}
		};

	SafeDestroyComponent(MainCameraComponent);
	SafeDestroyComponent(MainSpringArmComponent);
	SafeDestroyComponent(NetherCrownGhostTrailNiagaraComponent);
	SafeDestroyComponent(NetherCrownPostProcessComponent);
	SafeDestroyComponent(NetherCrownControlPPComponent);
	SafeDestroyComponent(NetherCrownControlGhostTrailComponent);
}

void ANetherCrownCharacter::MoveCharacter(const FInputActionValue& Value)
{
	if (!IsValid(Controller))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("Controller is invalid in %hs"), __FUNCTION__);
		return;
	}

	if (!bPressedMoveKey)
	{
		Server_SetPressedMoveKey(true);
	}

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
	if (bPressedMoveKey)
	{
		Server_SetPressedMoveKey(false);
	}
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

void ANetherCrownCharacter::SetIsHardLanding()
{
	const UNetherCrownCharacterDefaultSettings* CharacterDefaultSetting{ GetDefault<UNetherCrownCharacterDefaultSettings>() };
	check(CharacterDefaultSetting);

	const double DistanceBetweenLandAndJumpStartLocation{ JumpStartLocation.Z - GetActorLocation().Z };
	const double MinHardLandHeight{ CharacterDefaultSetting->GetMinHardLandingHeight() };
	bIsHardLanding = DistanceBetweenLandAndJumpStartLocation > MinHardLandHeight;
}

void ANetherCrownCharacter::ResetHardLandingState()
{
	bIsHardLanding = false;

	UCharacterMovementComponent* MovementComponent{ GetCharacterMovement() };
	check(MovementComponent);

	MovementComponent->SetMovementMode(MOVE_Walking);
}

void ANetherCrownCharacter::DisableMovementAndSetResetTimerWhenHardLanding()
{
	if (!IsValid(Controller))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("Controller is invalid in %hs"), __FUNCTION__);
		return;
	}

	if (bIsHardLanding)
	{
		UCharacterMovementComponent* MovementComponent{ GetCharacterMovement() };
		check(MovementComponent);

		MovementComponent->DisableMovement();

		const UNetherCrownCharacterDefaultSettings* CharacterDefaultSetting{ GetDefault<UNetherCrownCharacterDefaultSettings>() };
		check(CharacterDefaultSetting);

		const float ResetDelay = CharacterDefaultSetting->GetRecoveryResetDelayTime();
		GetWorldTimerManager().ClearTimer(TimerHandle_ResetHardLanding);
		GetWorldTimerManager().SetTimer(TimerHandle_ResetHardLanding, this, &ANetherCrownCharacter::ResetHardLandingState, ResetDelay, false);
	}
}

void ANetherCrownCharacter::SetCharacterMovementControl(const bool bEnableMovement) const
{
	UCharacterMovementComponent* MovementComponent{ GetCharacterMovement() };
	check(MovementComponent);

	bEnableMovement ? MovementComponent->SetMovementMode(MOVE_Walking) : MovementComponent->DisableMovement();
}

void ANetherCrownCharacter::OnRep_IsHardLanding()
{
	if (bIsHardLanding)
	{
		FNetherCrownUtilManager::PlaySound2DByGameplayTag(this, CharacterTagData.HardLandingSoundTag);
	}
}
