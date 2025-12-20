// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownWeapon.h"

#include "NetherCrownWeaponTraceComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownEquipComponent.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

ANetherCrownWeapon::ANetherCrownWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMeshComponent"));
	WeaponMeshComponent->SetupAttachment(RootComponent);

	WeaponHitBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponHitBoxComponent"));
	WeaponHitBoxComponent->SetupAttachment(RootComponent);

	WeaponEquipSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("WeaponEquipSphereComponent"));
	WeaponEquipSphereComponent->SetupAttachment(RootComponent);

	WeaponTraceComponent = CreateDefaultSubobject<UNetherCrownWeaponTraceComponent>(TEXT("WeaponTraceComponent"));

	bNetLoadOnClient = true;
	bReplicates = true;
	SetReplicatingMovement(true);
}

void ANetherCrownWeapon::SetWeaponHitTraceEnable(const bool bEnableWeaponHitTrace) const
{
	check(WeaponTraceComponent);
	WeaponTraceComponent->SetWeaponHitTraceEnable(bEnableWeaponHitTrace);
}

void ANetherCrownWeapon::InitWeaponTraceComponentSettings() const
{
	check(WeaponTraceComponent && WeaponMeshComponent);
	WeaponTraceComponent->InitWeaponTraceComponentSettings(WeaponMeshComponent->GetSocketLocation("TraceEnd"));
}

void ANetherCrownWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponData = FNetherCrownUtilManager::GetWeaponDataByGameplayTag(WeaponTag);

	check(WeaponEquipSphereComponent);
	WeaponEquipSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandleOnEquipSphereBeginOverlap);
	WeaponEquipSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::HandleOnEquipSphereEndOverlap);
}

void ANetherCrownWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANetherCrownWeapon::HandleOnEquipSphereBeginOverlap(UPrimitiveComponent* OnComponentBeginOverlap,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	//Server/Client 둘 다 호출
	if (HasAuthority())
	{
		SetEquipComponentSetting(OtherActor, true);
	}
}

void ANetherCrownWeapon::HandleOnEquipSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//Server/Client 둘 다 호출
	if (HasAuthority())
	{
		SetEquipComponentSetting(OtherActor, false);
	}
}

void ANetherCrownWeapon::SetEquipComponentSetting(AActor* OtherActor, const bool bCanEquip)
{
	ANetherCrownCharacter* NetherCrownCharacter{ Cast<ANetherCrownCharacter>(OtherActor) };
	if (!IsValid(NetherCrownCharacter))
	{
		return;
	}

	UNetherCrownEquipComponent* EquipComponent{ NetherCrownCharacter->GetEquipComponent() };
	if (!IsValid(EquipComponent))
	{
		return;
	}

	EquipComponent->SetCanEquip(bCanEquip);
	bCanEquip ? EquipComponent->SetEquipableWeapon(this) : EquipComponent->SetEquipableWeapon(nullptr);
}

void ANetherCrownWeapon::DisableEquipSphereCollision() const
{
	check(WeaponEquipSphereComponent);
	WeaponEquipSphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
