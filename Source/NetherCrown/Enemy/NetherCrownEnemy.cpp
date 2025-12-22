// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemy.h"

#include "Components/CapsuleComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownEnemyStatComponent.h"
#include "NetherCrown/Components/NetherCrownEquipComponent.h"
#include "NetherCrown/Data/NetherCrownWeaponData.h"

ANetherCrownEnemy::ANetherCrownEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	EnemyHitBoxComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("EnemyHitBoxComponent"));
	EnemyHitBoxComponent->SetupAttachment(RootComponent);

	EnemyStatComponent = CreateDefaultSubobject<UNetherCrownEnemyStatComponent>(TEXT("EnemyStatComponent"));
}

void ANetherCrownEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void ANetherCrownEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ANetherCrownEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ResultDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	const ANetherCrownCharacter* NetherCrownCharacter = Cast<ANetherCrownCharacter>(DamageCauser);
	if (!ensureAlways(IsValid(NetherCrownCharacter)))
	{
		return 0.f;
	}

	ProcessIncomingDamage(NetherCrownCharacter, ResultDamage);

	return ResultDamage;
}

void ANetherCrownEnemy::ProcessIncomingDamage(const ANetherCrownCharacter* DamageCauser, float DamageAmount)
{
	//@NOTE : This function is only executed by server
	if (!ensureAlways(IsValid(DamageCauser)))
	{
		return;
	}

	UNetherCrownEquipComponent* EquipComponent{ DamageCauser->GetEquipComponent() };
	if (!ensureAlways(IsValid(EquipComponent)))
	{
		return;
	}

	const UNetherCrownWeaponData* WeaponData{ EquipComponent->GetEquippedWeaponData() };
	if (!ensureAlways(IsValid(WeaponData)))
	{
		return;
	}

	const int32 PhysicalPenetration{ WeaponData->PhysicalPenetration };

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
