// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemy.h"

#include "NetherCrown/NetherCrown.h"
#include "NiagaraComponent.h"
#include "AnimInstance/NetherCrownEnemyAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/NetherCrownEnemyBasicAttackComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownCrowdControlComponent.h"
#include "NetherCrown/Components/NetherCrownEnemyStatComponent.h"
#include "NetherCrown/Components/NetherCrownEquipComponent.h"
#include "NetherCrown/Components/NetherCrownStatusEffectControlComponent.h"
#include "NetherCrown/Data/NetherCrownWeaponData.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"
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

	bNetLoadOnClient = true;
	bReplicates = true;

	SetEnemyMovementComponentValue();
}

UNetherCrownStatusEffectControlComponent* ANetherCrownEnemy::GetStatusEffectControlComponent() const
{
	return StatusEffectControlComponent;
}

void ANetherCrownEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority() && ensureAlways(IsValid(StatusNiagaraComponent)))
	{
		StatusEffectControlComponent->SetHandledStatusNiagaraComponent(StatusNiagaraComponent);
	}

	SetReplicateMovement(true);

	AttachEnemyWeapon();
}

float ANetherCrownEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//@NOTE : This function is only executed by server RPC
	float ResultDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Multicast_PlayTakeDamageSound();

	if (ensureAlways(IsValid(CrowdControlComponent)))
	{
		Multicast_PlayTakeDamageAnimation(CrowdControlComponent->GetCrowdControlType());
	}

	ProcessIncomingPhysicalDamage(DamageCauser, ResultDamage);

	return ResultDamage;
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

	CharacterMovementComponent->bOrientRotationToMovement = true;

	bUseControllerRotationYaw = false;
}

void ANetherCrownEnemy::ProcessIncomingPhysicalDamage(const AActor* DamageCauser, float DamageAmount)
{
	//@NOTE : This function is only executed by server RPC
	if (!HasAuthority())
	{
		return;
	}

	const ANetherCrownCharacter* NetherCrownCharacter = Cast<ANetherCrownCharacter>(DamageCauser);
	if (!ensureAlways(IsValid(NetherCrownCharacter)))
	{
		return;
	}

	UNetherCrownEquipComponent* EquipComponent{ NetherCrownCharacter->GetEquipComponent() };
	if (!ensureAlways(IsValid(EquipComponent)))
	{
		return;
	}

	const UNetherCrownWeaponData* WeaponData{ EquipComponent->GetEquippedWeaponData() };
	if (!ensureAlways(IsValid(WeaponData)))
	{
		return;
	}

	const int32 PhysicalPenetration{ WeaponData->GetPhysicalPenetration() };

	check(EnemyStatComponent);
	const FNetherCrownEnemyStat& EnemyStatData{ EnemyStatComponent->GetEnemyStatData() };
	const int32 EnemyArmor{ EnemyStatData.PhysicalArmor };

	const int32 EffectiveArmor{ FMath::Max(0, EnemyArmor - PhysicalPenetration) };
	const float DamageMultiplier{ 100.f / (100.f + EffectiveArmor)};
	const int32 FinalDamage{ FMath::RoundToInt(DamageAmount * DamageMultiplier) };

	EnemyStatComponent->SetEnemyHp(EnemyStatData.EnemyHP - FinalDamage);

	UE_LOG(LogNetherCrown, Warning, TEXT("FinalDamage : %d"), FinalDamage);
	if (EnemyStatData.EnemyHP <= 0)
	{
		//Destroy(); //@NOTE : Temp Code
	}
}

void ANetherCrownEnemy::Multicast_PlayTakeDamageAnimation_Implementation(const ENetherCrownCrowdControlType InCrowdControlType)
{
	if (HasAuthority())
	{
		return;
	}

	if (InCrowdControlType != ENetherCrownCrowdControlType::NONE)
	{
		return;
	}

	UAnimMontage* TakeDamageAnimMontage{ TakeDamageAnimMontageSoft.LoadSynchronous() };
	if (!ensureAlways(IsValid(TakeDamageAnimMontage)))
	{
		return;
	}

	const USkeletalMeshComponent* SkeletalMeshComponent{ GetMesh() };
	UNetherCrownEnemyAnimInstance* EnemyAnimInstance{ SkeletalMeshComponent ? Cast<UNetherCrownEnemyAnimInstance>(SkeletalMeshComponent->GetAnimInstance()) : nullptr };
	if (!ensureAlways(IsValid(EnemyAnimInstance)))
	{
		return;
	}

	EnemyAnimInstance->Montage_Play(TakeDamageAnimMontage);
}

void ANetherCrownEnemy::PlayTakeDamageSound() const
{
	if (HasAuthority())
	{
		return;
	}

	FNetherCrownUtilManager::PlaySound2DByGameplayTag(this, EnemyTagData.EnemyHurtGruntSoundTag);
	FNetherCrownUtilManager::PlaySound2DByGameplayTag(this, EnemyTagData.EnemyHurtImpactSoundTag);
}

void ANetherCrownEnemy::Multicast_PlayTakeDamageSound_Implementation()
{
	if (HasAuthority())
	{
		return;
	}

	PlayTakeDamageSound();
}
