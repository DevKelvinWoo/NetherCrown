// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemy.h"

#include "NiagaraComponent.h"
#include "AnimInstance/NetherCrownEnemyAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"

#include "Components/NetherCrownEnemyBasicAttackComponent.h"
#include "Components/NetherCrownEnemySkillComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownCrowdControlComponent.h"
#include "NetherCrown/Components/NetherCrownEnemyBTCosmeticComponent.h"
#include "NetherCrown/Components/NetherCrownEnemyDamageReceiverComponent.h"
#include "NetherCrown/Components/NetherCrownEnemyStatComponent.h"
#include "NetherCrown/Components/NetherCrownStatusEffectControlComponent.h"
#include "NetherCrown/Weapon/NetherCrownEnemyWeapon.h"

ANetherCrownEnemy::ANetherCrownEnemy()
{
	PrimaryActorTick.bCanEverTick = false;

	EnemyHitBoxComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("EnemyHitBoxComponent"));
	EnemyHitBoxComponent->SetupAttachment(RootComponent);

	StatusNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("StatusNiagaraComponent"));
	StatusNiagaraComponent->SetupAttachment(RootComponent);

	EnemyStatComponent = CreateDefaultSubobject<UNetherCrownEnemyStatComponent>(TEXT("EnemyStatComponent"));

	CrowdControlComponent = CreateDefaultSubobject<UNetherCrownCrowdControlComponent>(TEXT("CrowdControlComponent"));

	StatusEffectControlComponent = CreateDefaultSubobject<UNetherCrownStatusEffectControlComponent>(TEXT("StatusEffectControlComponent"));

	BasicAttackComponent = CreateDefaultSubobject<UNetherCrownEnemyBasicAttackComponent>(TEXT("BasicAttackComponent"));

	EnemyDamageReceiverComponent = CreateDefaultSubobject<UNetherCrownEnemyDamageReceiverComponent>(TEXT("EnemyDamageReceiverComponent"));

	EnemyBTCosmeticComponent = CreateDefaultSubobject<UNetherCrownEnemyBTCosmeticComponent>(TEXT("EnemyBTCosmeticComponent"));

	EnemySkillComponent = CreateDefaultSubobject<UNetherCrownEnemySkillComponent>(TEXT("EnemySkillComponent"));

	bNetLoadOnClient = true;
	bReplicates = true;

	SetEnemyMovementComponentValue();
}

UNetherCrownStatusEffectControlComponent* ANetherCrownEnemy::GetStatusEffectControlComponent() const
{
	return StatusEffectControlComponent;
}

void ANetherCrownEnemy::SetCurrentTargetCharacter(ANetherCrownCharacter* InTargetCharacter)
{
	CurrentTargetCharacterWeak = MakeWeakObjectPtr(InTargetCharacter);
}

void ANetherCrownEnemy::SetCharacterMaxSpeed(const bool bIsRunChase)
{
	UCharacterMovementComponent* CharacterMovementComponent{ GetCharacterMovement() };
	if (!ensureAlways(IsValid(CharacterMovementComponent)))
	{
		return;
	}

	if (!ensureAlways(IsValid(EnemyStatComponent)))
	{
		return;
	}

	const FNetherCrownEnemyStat& EnemyStat{ EnemyStatComponent->GetEnemyStatData() };
	const float Speed{ bIsRunChase ? EnemyStat.RunChaseSpeed : EnemyStat.DefaultSpeed };

	CharacterMovementComponent->MaxWalkSpeed = Speed;
}

void ANetherCrownEnemy::SetIsDead(const bool InbIsDead)
{
	if (HasAuthority())
	{
		bIsDead = InbIsDead;

		Multicast_SetAllMontageDisable();
		Multicast_SetHitBoxCollisionEnabled(false);
		Multicast_SetCapsuleCollisionResponse(ECC_Pawn, ECR_Ignore);
		Multicast_DeActiveStatusNiagaraSystem();
	}
}

void ANetherCrownEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() != NM_DedicatedServer && ensureAlways(IsValid(StatusNiagaraComponent)))
	{
		StatusEffectControlComponent->SetHandledStatusNiagaraComponent(StatusNiagaraComponent);
	}

	SetReplicateMovement(true);
}

float ANetherCrownEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//@NOTE : This function is only executed by server RPC
	float ResultDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (!ensureAlways(IsValid(EnemyDamageReceiverComponent)) || !HasAuthority())
	{
		return 0.f;
	}

	return EnemyDamageReceiverComponent->HandleIncomingDamage(ResultDamage, DamageEvent, DamageCauser);
}

void ANetherCrownEnemy::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsDead);
	DOREPLIFETIME(ThisClass, CurrentTargetCharacterWeak);
}

ENetherCrownCrowdControlType ANetherCrownEnemy::GetCrowdControlType() const
{
	if (!ensureAlways(IsValid(CrowdControlComponent)))
	{
		return ENetherCrownCrowdControlType::NONE;
	}

	return CrowdControlComponent->GetCrowdControlType();
}

void ANetherCrownEnemy::AttachEnemyWeapon()
{
	UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	FActorSpawnParameters SpawnParameters{};
	SpawnParameters.ObjectFlags = RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	EnemyWeapon = World->SpawnActor<ANetherCrownEnemyWeapon>(EnemyWeaponClass, SpawnParameters);
	if (!ensureAlways(IsValid(EnemyWeapon)))
	{
		return;
	}

	USkeletalMeshComponent* EnemyMesh{ GetMesh() };
	if (!ensureAlways(IsValid(EnemyMesh)))
	{
		return;
	}

	const FAttachmentTransformRules& AttachmentTransformRules{ EAttachmentRule::SnapToTarget, true };
	EnemyWeapon->AttachToComponent(EnemyMesh, AttachmentTransformRules, TEXT("WEAPON_R"));

	if (!ensureAlways(IsValid(BasicAttackComponent)))
	{
		return;
	}

	BasicAttackComponent->SetHandledEnemyWeapon(EnemyWeapon);
}

void ANetherCrownEnemy::SetEnemyMovementComponentValue()
{
	UCharacterMovementComponent* CharacterMovementComponent{ GetCharacterMovement() };
	if (!ensureAlways(IsValid(CharacterMovementComponent)))
	{
		return;
	}

	CharacterMovementComponent->MaxWalkSpeed = 300.f;
	CharacterMovementComponent->MaxAcceleration = 300.f;
	CharacterMovementComponent->BrakingDecelerationWalking = 1200.f;
	CharacterMovementComponent->bUseSeparateBrakingFriction = true;

	CharacterMovementComponent->bOrientRotationToMovement = true;

	bUseControllerRotationYaw = false;
}

void ANetherCrownEnemy::Multicast_SetAllMontageDisable_Implementation()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	const USkeletalMeshComponent* SkeletalMeshComponent{ GetMesh() };
	if (!ensureAlways(IsValid(SkeletalMeshComponent)))
	{
		return;
	}

	UNetherCrownEnemyAnimInstance* EnemyAnimInstance{ Cast<UNetherCrownEnemyAnimInstance>(SkeletalMeshComponent->GetAnimInstance()) };
	if (!ensureAlways(IsValid(EnemyAnimInstance)))
	{
		return;
	}

	EnemyAnimInstance->StopAllMontages(true);
}

void ANetherCrownEnemy::Multicast_DeActiveStatusNiagaraSystem_Implementation()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (!ensureAlways(IsValid(StatusEffectControlComponent)))
	{
		return;
	}

	StatusEffectControlComponent->SetActiveStatusNiagaraSystem(false);
}

void ANetherCrownEnemy::Multicast_SetCapsuleCollisionResponse_Implementation(const ECollisionChannel Channel, const ECollisionResponse Response)
{
	UCapsuleComponent* EnemyCapsuleComponent{ GetCapsuleComponent() };
	if (!ensureAlways(IsValid(EnemyCapsuleComponent)))
	{
		return;
	}

	EnemyCapsuleComponent->SetCollisionResponseToChannel(Channel, Response);
}

void ANetherCrownEnemy::Multicast_SetHitBoxCollisionEnabled_Implementation(const bool bEnable)
{
	if (!ensureAlways((IsValid(EnemyHitBoxComponent))))
	{
		return;
	}

	EnemyHitBoxComponent->SetCollisionEnabled(bEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}
