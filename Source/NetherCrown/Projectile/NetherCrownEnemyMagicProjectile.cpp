// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyMagicProjectile.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ANetherCrownEnemyMagicProjectile::ANetherCrownEnemyMagicProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	HitSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("HitSphereComponent"));
	SetRootComponent(HitSphereComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->UpdatedComponent = HitSphereComponent;

	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraComponent"));
	ProjectileNiagaraComponent->SetupAttachment(RootComponent);

	bNetLoadOnClient = true;
	bReplicates = true;
	SetReplicatingMovement(true);
}

void ANetherCrownEnemyMagicProjectile::InitProjectile(const FVector& InDirection, const float ProjectileSpeed)
{
	if (!ensureAlways(IsValid(ProjectileMovementComponent)) || !HasAuthority())
	{
		return;
	}

	ProjectileMovementComponent->InitialSpeed = ProjectileSpeed;
	ProjectileMovementComponent->MaxSpeed = ProjectileSpeed;
	ProjectileMovementComponent->Velocity = InDirection * ProjectileSpeed;
}

void ANetherCrownEnemyMagicProjectile::BeginPlay()
{
	Super::BeginPlay();
}
