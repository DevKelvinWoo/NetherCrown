// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownWeapon.h"

#include "NetherCrown/NetherCrown.h"
#include "NetherCrown/Data/NetherCrownWeaponData.h"
#include "NetherCrownWeaponTraceComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownBasicAttackComponent.h"
#include "NetherCrown/Components/NetherCrownEquipComponent.h"
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

void ANetherCrownWeapon::Multicast_ActiveWeaponLastComboAttackAuraNiagara_Implementation(const bool bActive)
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	ActiveWeaponLastComboAttackAuraNiagara(bActive);
}

void ANetherCrownWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponData = FNetherCrownUtilManager::GetWeaponDataByGameplayTag(WeaponTag);
	check(WeaponData);

	check(WeaponEquipSphereComponent);
	WeaponEquipSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandleOnEquipSphereBeginOverlap);
	WeaponEquipSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::HandleOnEquipSphereEndOverlap);

	check(WeaponTraceComponent);
	WeaponTraceComponent->GetOnHitEnemy().AddUObject(this, &ThisClass::HandleOnHitEnemy);

	CacheWeaponSkillAuraMap();
	CacheWeaponLastComboAttackAura();
}

void ANetherCrownWeapon::SetWeaponHitTraceEnable(const bool bEnableWeaponHitTrace) const
{
	if (!ensureAlways(IsValid(WeaponTraceComponent)) || !HasAuthority())
	{
		return;
	}

	WeaponTraceComponent->SetWeaponHitTraceEnable(bEnableWeaponHitTrace);
}

void ANetherCrownWeapon::InitWeaponTraceComponentSettings() const
{
	if (!ensureAlways(IsValid(WeaponTraceComponent)) || !ensureAlways(IsValid(WeaponMeshComponent)) || !ensureAlways(IsValid(WeaponData)))
	{
		return;
	}

	const FName& TraceSocketName{ WeaponData->GetWeaponTraceData().WeaponTraceSocketName };
	WeaponTraceComponent->InitWeaponTraceComponentSettings(WeaponMeshComponent->GetSocketLocation(TraceSocketName));
}

void ANetherCrownWeapon::SetTraceMode(const ENetherCrownTraceMode InTraceMode)
{
	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)) || !OwnerCharacter->HasAuthority())
	{
		return;
	}

	if (!ensureAlways(IsValid(WeaponTraceComponent)))
	{
		return;
	}

	WeaponTraceComponent->SetTraceMode(InTraceMode);
}

void ANetherCrownWeapon::ActiveWeaponSkillAuraNiagara(const bool bActive, const ENetherCrownSkillKeyEnum SkillKeyEnum) const
{
	if (CachedWeaponSkillAuraMap.IsEmpty())
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("CachedWeaponAuraMap is Empty in %hs"), __FUNCTION__);
		return;
	}

	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)) || OwnerCharacter->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (!ensureAlways(IsValid(WeaponAuraNiagaraComponent)))
	{
		return;
	}

	if (bActive && SkillKeyEnum != ENetherCrownSkillKeyEnum::None)
	{
		const TObjectPtr<UNiagaraSystem>* FoundWeaponAuraNiagaraSystem = CachedWeaponSkillAuraMap.Find(SkillKeyEnum);
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

void ANetherCrownWeapon::ActiveWeaponLastComboAttackAuraNiagara(const bool bActive) const
{
	if (!ensureAlways(IsValid(CachedWeaponLastComboAttackAura)))
	{
		return;
	}

	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)) || OwnerCharacter->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (!ensureAlways(IsValid(WeaponAuraNiagaraComponent)))
	{
		return;
	}

	if (bActive)
	{
		if (CachedWeaponLastComboAttackAura)
		{
			WeaponAuraNiagaraComponent->SetAsset(CachedWeaponLastComboAttackAura);
			WeaponAuraNiagaraComponent->Activate();
		}
	}
	else
	{
		WeaponAuraNiagaraComponent->SetAsset(nullptr);
		WeaponAuraNiagaraComponent->Deactivate();
	}
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
	if (!ensureAlways(IsValid(NetherCrownCharacter)) || !NetherCrownCharacter->HasAuthority())
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
		UE_LOG(LogNetherCrown, Warning, TEXT("HitEnemy is invalid %hs"), __FUNCTION__);
		return;
	}

	const ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)) || !OwnerCharacter->HasAuthority())
	{
		return;
	}

	UNetherCrownBasicAttackComponent* BasicAttackComponent{ OwnerCharacter->GetBasicAttackComponent() };
	if (!ensureAlways(IsValid(BasicAttackComponent)))
	{
		return;
	}

	BasicAttackComponent->ApplyHitCosmeticAndDamageToHitEnemy(HitEnemy, HitLocation);
}

void ANetherCrownWeapon::Multicast_ActiveWeaponSkillAuraNiagara_Implementation(const bool bActive, const ENetherCrownSkillKeyEnum SkillKeyEnum)
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	ActiveWeaponSkillAuraNiagara(bActive, SkillKeyEnum);
}

void ANetherCrownWeapon::CacheWeaponSkillAuraMap()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (!ensureAlways(IsValid(WeaponData)))
	{
		return;
	}

	for (const auto& WeaponAuraPair : WeaponData->GetWeaponSkillAuraMap())
	{
		CachedWeaponSkillAuraMap.Add(WeaponAuraPair.Key, WeaponAuraPair.Value.LoadSynchronous());
	}
}

void ANetherCrownWeapon::CacheWeaponLastComboAttackAura()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (!ensureAlways(IsValid(WeaponData)))
	{
		return;
	}

	CachedWeaponLastComboAttackAura = WeaponData->GetWeaponLastComboAttackAura().LoadSynchronous();
}

void ANetherCrownWeapon::DisableEquipSphereCollision() const
{
	if (!HasAuthority())
	{
		return;
	}

	if (!ensureAlways(IsValid(WeaponEquipSphereComponent)))
	{
		return;
	}

	WeaponEquipSphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
