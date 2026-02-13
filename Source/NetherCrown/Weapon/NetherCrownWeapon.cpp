// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownWeapon.h"

#include "NetherCrownWeaponTraceComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownBasicAttackComponent.h"
#include "NetherCrown/Components/NetherCrownEquipComponent.h"
#include "NetherCrown/Settings/NetherCrownCharacterDefaultSettings.h"
#include "NetherCrown/Skill/NetherCrownSkillObject.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"

ANetherCrownWeapon::ANetherCrownWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMeshComponent"));
	WeaponMeshComponent->SetupAttachment(RootComponent);

	WeaponEquipSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("WeaponEquipSphereComponent"));
	WeaponEquipSphereComponent->SetupAttachment(RootComponent);

	WeaponTraceComponent = CreateDefaultSubobject<UNetherCrownWeaponTraceComponent>(TEXT("WeaponTraceComponent"));

	WeaponAuraNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WeaponAuraNiagaraComponent"));
	WeaponAuraNiagaraComponent->SetupAttachment(WeaponMeshComponent);

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

	const UNetherCrownCharacterDefaultSettings* CharacterDefaultSettings = GetDefault<UNetherCrownCharacterDefaultSettings>();
	check(CharacterDefaultSettings);

	const FName& TraceSocketName = CharacterDefaultSettings->GetWeaponTraceSocketName();
	WeaponTraceComponent->InitWeaponTraceComponentSettings(WeaponMeshComponent->GetSocketLocation(TraceSocketName));
}

void ANetherCrownWeapon::ActiveWeaponAuraNiagara(const bool bActive, const ENetherCrownSkillKeyEnum SkillKeyEnum) const
{
	if (CachedWeaponAuraMap.IsEmpty())
	{
		return;
	}

	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)))
	{
		return;
	}

	if (OwnerCharacter->HasAuthority())
	{
		return;
	}

	check(WeaponAuraNiagaraComponent);

	if (bActive && SkillKeyEnum != ENetherCrownSkillKeyEnum::None)
	{
		const TObjectPtr<UNiagaraSystem>* FoundWeaponAuraNiagaraSystem = CachedWeaponAuraMap.Find(SkillKeyEnum);
		if (FoundWeaponAuraNiagaraSystem)
		{
			WeaponAuraNiagaraComponent->SetAsset(*FoundWeaponAuraNiagaraSystem);
			WeaponAuraNiagaraComponent->Activate();
		}
	}
	else
	{
		WeaponAuraNiagaraComponent->SetAsset(nullptr);
		WeaponAuraNiagaraComponent->Deactivate();
	}
}

void ANetherCrownWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponData = FNetherCrownUtilManager::GetWeaponDataByGameplayTag(WeaponTagData.WeaponTag);

	check(WeaponEquipSphereComponent);
	WeaponEquipSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandleOnEquipSphereBeginOverlap);
	WeaponEquipSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::HandleOnEquipSphereEndOverlap);

	check(WeaponTraceComponent);
	WeaponTraceComponent->GetOnHitEnemy().AddUObject(this, &ThisClass::HandleOnHitEnemy);

	CacheWeaponAuraMap();
}

void ANetherCrownWeapon::HandleOnEquipSphereBeginOverlap(UPrimitiveComponent* OnComponentBeginOverlap,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!(OtherActor->IsA<ANetherCrownCharacter>()))
	{
		return;
	}

	//Server/Client 둘 다 호출
	if (HasAuthority())
	{
		SetEquipComponentSetting(OtherActor, true);
	}
}

void ANetherCrownWeapon::HandleOnEquipSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!(OtherActor->IsA<ANetherCrownCharacter>()))
	{
		return;
	}

	//Server/Client 둘 다 호출
	if (HasAuthority())
	{
		SetEquipComponentSetting(OtherActor, false);
	}
}

void ANetherCrownWeapon::SetEquipComponentSetting(AActor* OtherActor, const bool bCanEquip)
{
	ANetherCrownCharacter* NetherCrownCharacter{ Cast<ANetherCrownCharacter>(OtherActor) };
	if (!ensureAlways(IsValid(NetherCrownCharacter)))
	{
		return;
	}

	UNetherCrownEquipComponent* EquipComponent{ NetherCrownCharacter->GetEquipComponent() };
	if (!ensureAlways(IsValid(EquipComponent)))
	{
		return;
	}

	EquipComponent->SetCanEquip(bCanEquip);
	bCanEquip ? EquipComponent->SetEquipableWeapon(this) : EquipComponent->SetEquipableWeapon(nullptr);
}

void ANetherCrownWeapon::HandleOnHitEnemy(AActor* HitEnemy, const FVector& HitLocation) const
{
	if (!IsValid(HitEnemy))
	{
		UE_LOG(LogTemp, Warning, TEXT("HitEnemy is invalid %hs"), __FUNCTION__);
		return;
	}

	const ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)))
	{
		UE_LOG(LogTemp, Warning, TEXT("OwnerCharacter is invalid %hs"), __FUNCTION__);
		return;
	}

	UNetherCrownBasicAttackComponent* BasicAttackComponent{ OwnerCharacter->GetBasicAttackComponent() };
	if (!ensureAlways(IsValid(BasicAttackComponent)))
	{
		UE_LOG(LogTemp, Warning, TEXT("BasicAttackComponent is invalid %hs"), __FUNCTION__);
		return;
	}

	BasicAttackComponent->ApplyDamageToHitEnemy(HitEnemy, HitLocation);
}

void ANetherCrownWeapon::Multicast_ActiveWeaponAuraNiagara_Implementation(const bool bActive, const ENetherCrownSkillKeyEnum SkillKeyEnum)
{
	ActiveWeaponAuraNiagara(bActive, SkillKeyEnum);
}

void ANetherCrownWeapon::CacheWeaponAuraMap()
{
	for (const auto& WeaponAuraPair : WeaponAuraMap)
	{
		CachedWeaponAuraMap.Add(WeaponAuraPair.Key, WeaponAuraPair.Value.LoadSynchronous());
	}
}

void ANetherCrownWeapon::DisableEquipSphereCollision() const
{
	check(WeaponEquipSphereComponent);
	WeaponEquipSphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
