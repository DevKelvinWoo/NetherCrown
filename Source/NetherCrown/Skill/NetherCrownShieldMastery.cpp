// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownShieldMastery.h"

#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"
#include "NiagaraComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownPlayerStatComponent.h"
#include "NetherCrown/PlayerState/NetherCrownPlayerState.h"
#include "SkillActors/NetherCrownShield.h"

UNetherCrownShieldMastery::UNetherCrownShieldMastery()
{
	SetSkillTag(NetherCrownTags::Skill_ShieldMastery);
}

void UNetherCrownShieldMastery::InitSkillObject()
{
	Super::InitSkillObject();

	CacheShieldMasteryData();
}

void UNetherCrownShieldMastery::CacheShieldMasteryData()
{
	const UNetherCrownSkillDataAsset* SkillDataAsset{ FNetherCrownUtilManager::GetSkillDataAssetByGameplayTag(NetherCrownTags::Skill_ShieldMastery) };
	const UNetherCrownShieldMasteryDataAsset* ShieldMasteryDataAsset{ Cast<UNetherCrownShieldMasteryDataAsset>(SkillDataAsset) };
	if (!ensureAlways(IsValid(ShieldMasteryDataAsset)))
	{
		return;
	}

	ShieldMasteryData = ShieldMasteryDataAsset->GetShieldMasteryData();
}

void UNetherCrownShieldMastery::PlaySkillCosmetics()
{
	Super::PlaySkillCosmetics();

	ActiveShieldEffectAndActor();
	PlayShieldOnSound();
}

void UNetherCrownShieldMastery::ExecuteSkillGameplay()
{
	Super::ExecuteSkillGameplay();

	AddPlayerShieldAndSetShieldEndTimer(ShieldMasteryData.ShieldValue);
}

void UNetherCrownShieldMastery::ActiveShieldEffectAndActor()
{
	UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || SkillOwnerCharacter->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	USkeletalMeshComponent* SkeletalMeshComponent{ SkillOwnerCharacter->GetMesh() };
	if (!ensureAlways(IsValid(SkeletalMeshComponent)))
	{
		return;
	}

	HandledShieldMasteryNiagaraComponent = FNetherCrownUtilManager::AttachNiagaraSystemByGameplayTag(World, NetherCrownTags::Effect_ShieldMastery, SkeletalMeshComponent, ShieldMasteryData.ShieldEffectSocketName);

	World->GetTimerManager().ClearTimer(ShieldMasteryDeactivateTimerHandle);
	World->GetTimerManager().SetTimer(ShieldMasteryDeactivateTimerHandle, this, &ThisClass::DeactivateShieldEffectAndActor, ShieldMasteryData.ShieldDuration, false);

	HandledShieldMasteryActor = World->SpawnActor<ANetherCrownShield>(ShieldMasteryData.ShieldActorClass);
	if (!ensureAlways(IsValid(HandledShieldMasteryActor)))
	{
		return;
	}

	HandledShieldMasteryActor->AttachToActor(SkillOwnerCharacter, FAttachmentTransformRules::SnapToTargetNotIncludingScale, ShieldMasteryData.ShieldEffectSocketName);
}

void UNetherCrownShieldMastery::DeactivateShieldEffectAndActor()
{
	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || SkillOwnerCharacter->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (const UWorld* World{ GetWorld() })
	{
		World->GetTimerManager().ClearTimer(ShieldMasteryDeactivateTimerHandle);
	}

	if (!(IsValid(HandledShieldMasteryNiagaraComponent)))
	{
		return;
	}

	HandledShieldMasteryNiagaraComponent->DestroyComponent();
	HandledShieldMasteryNiagaraComponent = nullptr;

	if (!(IsValid(HandledShieldMasteryActor)))
	{
		return;
	}

	HandledShieldMasteryActor->DestroyShield();
}

void UNetherCrownShieldMastery::PlayShieldOnSound() const
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || SkillOwnerCharacter->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	FNetherCrownUtilManager::PlaySound2DByGameplayTag(GetWorld(), NetherCrownTags::Sound_Shield_On);
}

void UNetherCrownShieldMastery::AddPlayerShieldAndSetShieldEndTimer(const int32 InShieldValue)
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || !SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	ANetherCrownPlayerState* PlayerState{ Cast<ANetherCrownPlayerState>(SkillOwnerCharacter->GetPlayerState()) };
	if (!ensureAlways(IsValid(PlayerState)))
	{
		return;
	}

	UNetherCrownPlayerStatComponent* PlayerStatComponent{ PlayerState->GetNetherCrownPlayerStatComponent() };
	if (!ensureAlways(IsValid(PlayerStatComponent)))
	{
		return;
	}

	PlayerStatComponent->AddPlayerShield(InShieldValue);

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	World->GetTimerManager().ClearTimer(ShieldMasteryClearPlayerShieldTimerHandle);
	World->GetTimerManager().SetTimer(ShieldMasteryClearPlayerShieldTimerHandle, this, &ThisClass::ClearPlayerShield, ShieldMasteryData.ShieldDuration, false);
}

void UNetherCrownShieldMastery::ClearPlayerShield()
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || !SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	if (const UWorld* World{ GetWorld() })
	{
		World->GetTimerManager().ClearTimer(ShieldMasteryClearPlayerShieldTimerHandle);
	}

	ANetherCrownPlayerState* PlayerState{ Cast<ANetherCrownPlayerState>(SkillOwnerCharacter->GetPlayerState()) };
	if (!ensureAlways(IsValid(PlayerState)))
	{
		return;
	}

	UNetherCrownPlayerStatComponent* PlayerStatComponent{ PlayerState->GetNetherCrownPlayerStatComponent() };
	if (!ensureAlways(IsValid(PlayerStatComponent)))
	{
		return;
	}

	PlayerStatComponent->ClearPlayerShield();
}
