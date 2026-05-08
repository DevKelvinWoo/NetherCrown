// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyMagicProjectile.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

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

void ANetherCrownEnemyMagicProjectile::InitProjectile(const FNetherCrownEnemyMagicProjectileInitData& InitData)
{
	if (!ensureAlways(IsValid(ProjectileMovementComponent)) || !HasAuthority())
	{
		return;
	}

	ProjectileMovementComponent->InitialSpeed = InitData.ProjectileSpeed;
	ProjectileMovementComponent->MaxSpeed = InitData.ProjectileSpeed;
	ProjectileMovementComponent->Velocity = InitData.Direction * InitData.ProjectileSpeed;

	DestroyNiagaraEffectTag = InitData.DestroyNiagaraEffectTag;
	DestroySoundTag = InitData.DestroySoundTag;
}

void ANetherCrownEnemyMagicProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (ensureAlways(IsValid(HitSphereComponent)) && HasAuthority())
	{
		HitSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandleOnHitSphereBeginOverlap);
	}
}

void ANetherCrownEnemyMagicProjectile::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, DestroyNiagaraEffectTag);
	DOREPLIFETIME(ThisClass, DestroySoundTag);
}

void ANetherCrownEnemyMagicProjectile::HandleOnHitSphereBeginOverlap(UPrimitiveComponent* OnComponentBeginOverlap,
                                                                     AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                                     const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!IsValid(OtherActor) || OtherActor == this || OtherActor == GetOwner() || OtherActor == GetInstigator())
	{
		return;
	}

	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(OtherActor) };
	if (!IsValid(OwnerCharacter))
	{
		return;
	}

	OnMagicProjectileHit.Broadcast(OwnerCharacter, this);

	const FTransform& DestroyNiagaraSystemTransform{ FTransform(FRotator(), SweepResult.Location, FVector::OneVector) };
	//Multicast_SpawnDestroyProjectileEffect(DestroyNiagaraSystemTransform);
	//Multicast_PlayDestroyProjectileSound();

	Destroy();
}

void ANetherCrownEnemyMagicProjectile::Multicast_PlayDestroyProjectileSound_Implementation()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	FNetherCrownUtilManager::PlaySound2DByGameplayTag(this, DestroySoundTag);
}

void ANetherCrownEnemyMagicProjectile::Multicast_SpawnDestroyProjectileEffect_Implementation(const FTransform& DestroyTransform)
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	FNetherCrownUtilManager::SpawnNiagaraSystemByGameplayTag(this, DestroyNiagaraEffectTag, DestroyTransform);
}
