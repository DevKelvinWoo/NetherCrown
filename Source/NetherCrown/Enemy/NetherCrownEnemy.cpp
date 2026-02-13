// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemy.h"

#include "AnimInstance/NetherCrownEnemyAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownCrowdControlComponent.h"
#include "NetherCrown/Components/NetherCrownEnemyStatComponent.h"
#include "NetherCrown/Components/NetherCrownEquipComponent.h"
#include "NetherCrown/Data/NetherCrownWeaponData.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

ANetherCrownEnemy::ANetherCrownEnemy()
{
	PrimaryActorTick.bCanEverTick = false;

	EnemyHitBoxComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("EnemyHitBoxComponent"));
	EnemyHitBoxComponent->SetupAttachment(RootComponent);

	EnemyStatComponent = CreateDefaultSubobject<UNetherCrownEnemyStatComponent>(TEXT("EnemyStatComponent"));

	CrowdControlComponent = CreateDefaultSubobject<UNetherCrownCrowdControlComponent>(TEXT("CrowdControlComponent"));

	bNetLoadOnClient = true;
	bReplicates = true;
}

void ANetherCrownEnemy::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(true);
}

float ANetherCrownEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//@NOTE : This function is only executed by server RPC
	float ResultDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Multicast_PlayTakeDamageSound();

	check(CrowdControlComponent);
	Multicast_PlayTakeDamageAnimation(CrowdControlComponent->GetCrowdControlType());

	ProcessIncomingPhysicalDamage(DamageCauser, ResultDamage);

	return ResultDamage;
}

void ANetherCrownEnemy::ProcessIncomingPhysicalDamage(const AActor* DamageCauser, float DamageAmount)
{
	//@NOTE : This function is only executed by server RPC
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
	const FNetherCrownEnemyStatData& EnemyStatData{ EnemyStatComponent->GetEnemyStatData() };
	const int32 EnemyArmor{ EnemyStatData.PhysicalArmor };

	const int32 EffectiveArmor{ FMath::Max(0, EnemyArmor - PhysicalPenetration) };
	const float DamageMultiplier{ 100.f / (100.f + EffectiveArmor)};
	const int32 FinalDamage{ FMath::RoundToInt(DamageAmount * DamageMultiplier) };

	EnemyStatComponent->SetEnemyHp(EnemyStatData.EnemyHP - FinalDamage);

	UE_LOG(LogTemp, Warning, TEXT("FinalDamage : %d"), FinalDamage);
	if (EnemyStatData.EnemyHP <= 0)
	{
		//Destroy(); //@NOTE : Temp Code
	}
}

void ANetherCrownEnemy::Multicast_PlayTakeDamageAnimation_Implementation(const ENetherCrownCrowdControlType InCrowdControlType)
{
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
	FNetherCrownUtilManager::PlaySound2DByGameplayTag(this, EnemyTagData.EnemyHurtGruntSoundTag);
	FNetherCrownUtilManager::PlaySound2DByGameplayTag(this, EnemyTagData.EnemyHurtImpactSoundTag);
}

void ANetherCrownEnemy::ApplyCrowdControl(const ENetherCrownCrowdControlType InCrowdControlType, float DurationTime)
{
	if (!HasAuthority())
	{
		return;
	}

	check(CrowdControlComponent);
	CrowdControlComponent->ApplyCrowdControl(InCrowdControlType, DurationTime);
}

void ANetherCrownEnemy::Multicast_PlayTakeDamageSound_Implementation()
{
	PlayTakeDamageSound();
}
