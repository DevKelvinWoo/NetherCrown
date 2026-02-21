// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownCharacterAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownBasicAttackComponent.h"
#include "NetherCrown/Components/NetherCrownEquipComponent.h"
#include "NetherCrown/Components/NetherCrownSkillComponent.h"
#include "NetherCrown/Weapon/NetherCrownWeapon.h"

void UNetherCrownCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	CacheInitData();
}

void UNetherCrownCharacterAnimInstance::CacheInitData()
{
	CachedOwningCharacter = Cast<ANetherCrownCharacter>(GetOwningActor());

	CachedBasicAttackComponent = CachedOwningCharacter->GetBasicAttackComponent();
	CachedEquipComponent = CachedOwningCharacter->GetEquipComponent();
	CachedSkillComponent = CachedOwningCharacter->GetSkillComponent();
	CachedCharacterMovementComponent = CachedOwningCharacter->GetCharacterMovement();
}

void UNetherCrownCharacterAnimInstance::AnimNotify_ComboEnable()
{
	if (!(IsValid(CachedOwningCharacter)) || !(IsValid(CachedBasicAttackComponent)))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedOwningCharacter or CachedBasicAttackComponent is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	CachedBasicAttackComponent->HandleEnableComboWindow();
}

void UNetherCrownCharacterAnimInstance::AnimNotify_ComboDisable()
{
	if (!(IsValid(CachedOwningCharacter)) || !(IsValid(CachedBasicAttackComponent)))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedOwningCharacter or CachedBasicAttackComponent is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	CachedBasicAttackComponent->HandleDisableComboWindow();
}

void UNetherCrownCharacterAnimInstance::AnimNotify_BasicAttackEnd()
{
	if (!(IsValid(CachedOwningCharacter)) || !(IsValid(CachedBasicAttackComponent)))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedOwningCharacter or CachedBasicAttackComponent is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	CachedBasicAttackComponent->HandleBasicAttackEnd();
}

void UNetherCrownCharacterAnimInstance::AnimNotify_EquipStart()
{
	if (!(IsValid(CachedOwningCharacter)) || !(IsValid(CachedEquipComponent)))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedOwningCharacter or CachedEquipComponent is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	CachedEquipComponent->NotifyEquipEndOrStart(false);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_EquipEnd()
{
	if (!(IsValid(CachedOwningCharacter)) || !(IsValid(CachedEquipComponent)))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedOwningCharacter or CachedEquipComponent is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	CachedEquipComponent->NotifyEquipEndOrStart(true);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_HitTraceEnable()
{
	if (!(IsValid(CachedOwningCharacter)) || !(IsValid(CachedBasicAttackComponent)))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedOwningCharacter or CachedBasicAttackComponent is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	CachedBasicAttackComponent->HandleEnableHitTrace();
}

void UNetherCrownCharacterAnimInstance::AnimNotify_SkillSlowBegin()
{
	if (!(IsValid(CachedOwningCharacter)) || !(IsValid(CachedSkillComponent)))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedOwningCharacter or CachedSkillComponent is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	CachedSkillComponent->SetActiveSkillSlowPlayRate(true);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_SkillSlowEnd()
{
	if (!(IsValid(CachedOwningCharacter)) || !(IsValid(CachedSkillComponent)))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedOwningCharacter or CachedSkillComponent is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	CachedSkillComponent->SetActiveSkillSlowPlayRate(false);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_SkillStart()
{
	if (!(IsValid(CachedOwningCharacter)) || !(IsValid(CachedSkillComponent)) || !(IsValid(CachedBasicAttackComponent)))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedOwningCharacter or CachedSkillComponent is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	CachedBasicAttackComponent->SetCanAttack(false);
	CachedSkillComponent->GetOnStopOrStartSkill().Broadcast(false);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_SkillEnd()
{
	if (!(IsValid(CachedOwningCharacter)) || !(IsValid(CachedSkillComponent)) || !(IsValid(CachedBasicAttackComponent)))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedOwningCharacter or CachedSkillComponent is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	CachedBasicAttackComponent->SetCanAttack(true);
	CachedSkillComponent->GetOnStopOrStartSkill().Broadcast(true);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_SetCharacterMovementFly()
{
	if (!(IsValid(CachedOwningCharacter)) || !(IsValid(CachedCharacterMovementComponent)))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedOwningCharacter or CachedCharacterMovementComponent is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	CachedCharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Flying);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_SetCharacterMovementWalk()
{
	if (!(IsValid(CachedOwningCharacter)) || !(IsValid(CachedCharacterMovementComponent)))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedOwningCharacter or CachedCharacterMovementComponent is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	CachedCharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_ActiveWeaponAuraNiagara()
{
	if (!(IsValid(CachedOwningCharacter)))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedOwningCharacter is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	if (!(IsValid(CachedSkillComponent)))
	{
		UE_LOG(LogTemp, Warning, TEXT("CachedSkillComponent is not valid %hs"), __FUNCTION__);
		return;
	}

	ANetherCrownWeapon* EquippedWeapon{ CachedEquipComponent ? CachedEquipComponent->GetEquippedWeapon() : nullptr };
	if (!(IsValid(EquippedWeapon)))
	{
		UE_LOG(LogTemp, Warning, TEXT("EquippedWeapon is not valid %hs"), __FUNCTION__);
		return;
	}

	const ENetherCrownSkillKeyEnum SkillKeyEnum{ CachedSkillComponent->GetActiveSkillKeyEnum() };
	EquippedWeapon->Multicast_ActiveWeaponAuraNiagara(true, SkillKeyEnum);
}

void UNetherCrownCharacterAnimInstance::AnimNotify_DeactiveWeaponAuraNiagara()
{
	if (!(IsValid(CachedOwningCharacter)))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedOwningCharacter is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	ANetherCrownWeapon* EquippedWeapon{ CachedEquipComponent ? CachedEquipComponent->GetEquippedWeapon() : nullptr };
	if (!(IsValid(EquippedWeapon)))
	{
		UE_LOG(LogTemp, Warning, TEXT("EquippedWeapon is not valid %hs"), __FUNCTION__);
		return;
	}

	EquippedWeapon->Multicast_ActiveWeaponAuraNiagara(false, ENetherCrownSkillKeyEnum::None);
}
