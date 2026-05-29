// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyMagicProjectile.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/DamageTypes/NetherCrownDamageEvent.h"
#include "NetherCrown/DamageTypes/NetherCrownMagicDamageType.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
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

	OwnerEnemy = InitData.OwnerEnemyWeak.Get();
	ReflectedOwnerCharacter = InitData.ReflectedOwnerCharacterWeak.Get();

	DestroyNiagaraEffectTag = InitData.DestroyNiagaraEffectTag;
	DestroySoundTag = InitData.DestroySoundTag;
	HitImpactEffectTag = InitData.HitImpactEffectTag;
	HitImpactSoundTag = InitData.HitImpactSoundTag;
	ProjectileDamage = InitData.ProjectileDamage;
	bReflected = InitData.bReflected;
}

float ANetherCrownEnemyMagicProjectile::GetProjectileSpeed() const
{
	if (!ensureAlways(IsValid(ProjectileMovementComponent)))
	{
		return 0.f;
	}

	return ProjectileMovementComponent->MaxSpeed;
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
	DOREPLIFETIME(ThisClass, OwnerEnemy);
	DOREPLIFETIME(ThisClass, ReflectedOwnerCharacter);
	DOREPLIFETIME(ThisClass, HitImpactEffectTag);
	DOREPLIFETIME(ThisClass, HitImpactSoundTag);
	DOREPLIFETIME(ThisClass, ProjectileDamage);
	DOREPLIFETIME(ThisClass, bReflected);
}

void ANetherCrownEnemyMagicProjectile::HandleOnHitSphereBeginOverlap(UPrimitiveComponent* OnComponentBeginOverlap,
                                                                     AActor* OtherActor, UPrimitiveComponent* OtherComp, const int32 OtherBodyIndex, const bool bFromSweep,
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

	if (bReflected)
	{
		ANetherCrownEnemy* HitEnemy{ Cast<ANetherCrownEnemy>(OtherActor) };
		if (!IsValid(HitEnemy))
		{
			return;
		}

		FNetherCrownDamageEvent::ApplyDamage(
			HitEnemy,
			ProjectileDamage,
			IsValid(ReflectedOwnerCharacter) ? ReflectedOwnerCharacter->GetController() : nullptr,
			this,
			UNetherCrownMagicDamageType::StaticClass(),
			HitImpactSoundTag,
			HitImpactEffectTag
		);

		const FTransform& HitImpactTransform{ FTransform(FRotator(), SweepResult.Location, FVector::OneVector) };
		Multicast_SpawnHitImpactEffect(HitImpactTransform);

		const FTransform& DestroyNiagaraSystemTransform{ HitImpactTransform };
		//Multicast_SpawnDestroyProjectileEffect(DestroyNiagaraSystemTransform);
		//Multicast_PlayDestroyProjectileSound();

		Destroy();
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

void ANetherCrownEnemyMagicProjectile::Multicast_SpawnHitImpactEffect_Implementation(const FTransform& HitImpactTransform)
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	FNetherCrownUtilManager::SpawnNiagaraSystemByGameplayTag(this, HitImpactEffectTag, HitImpactTransform);
}
