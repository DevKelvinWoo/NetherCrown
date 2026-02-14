// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownShieldMastery.h"

#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"
#include "NiagaraComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownPlayerStatComponent.h"
#include "NetherCrown/PlayerState/NetherCrownPlayerState.h"
#include "SkillActors/NetherCrownShield.h"

void UNetherCrownShieldMastery::PlaySkillCosmetics()
{
	Super::PlaySkillCosmetics();

	ActiveShieldEffectAndActor();
	PlayShieldOnSound();
}

void UNetherCrownShieldMastery::ExecuteSkillGameplay()
{
	Super::ExecuteSkillGameplay();

	AddPlayerShieldAndSetShieldEndTimer(ShieldValue);
}

void UNetherCrownShieldMastery::ActiveShieldEffectAndActor()
{
	UWorld* World{ GetWorld() };
	check(World);

	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	if (SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	USkeletalMeshComponent* SkeletalMeshComponent{ SkillOwnerCharacter->GetMesh() };
	check(SkeletalMeshComponent)

	HandledShieldMasteryNiagaraComponent = FNetherCrownUtilManager::AttachNiagaraSystemByGameplayTag(World, NetherCrownTags::Effect_ShieldMastery, SkeletalMeshComponent, ShieldEffectSocketName);

	FTimerHandle ShieldMasteryTimerHandle{};
	World->GetTimerManager().SetTimer(ShieldMasteryTimerHandle, this, &ThisClass::DeactivateShieldEffectAndActor, ShieldDuration, false);

	HandledShieldMasteryActor = World->SpawnActor<ANetherCrownShield>(ShieldActorClass);
	if (!ensureAlways(IsValid(HandledShieldMasteryActor)))
	{
		return;
	}

	HandledShieldMasteryActor->AttachToActor(SkillOwnerCharacter, FAttachmentTransformRules::SnapToTargetNotIncludingScale, ShieldEffectSocketName);
}

void UNetherCrownShieldMastery::DeactivateShieldEffectAndActor()
{
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
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	if (SkillOwnerCharacter->IsLocallyControlled())
	{
		FNetherCrownUtilManager::PlaySound2DByGameplayTag(GetWorld(), NetherCrownTags::Sound_Shield_On);
	}
}

void UNetherCrownShieldMastery::AddPlayerShieldAndSetShieldEndTimer(int32 InShieldValue) const
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
	check(PlayerStatComponent);

	PlayerStatComponent->AddPlayerShield(InShieldValue);

	const UWorld* World{ GetWorld() };
	check(World);

	FTimerHandle ShieldMasteryEndTimerHandle{};
	World->GetTimerManager().SetTimer(ShieldMasteryEndTimerHandle, this, &ThisClass::ClearPlayerShield, ShieldDuration, false);
}

void UNetherCrownShieldMastery::ClearPlayerShield() const
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	ANetherCrownPlayerState* PlayerState{ Cast<ANetherCrownPlayerState>(SkillOwnerCharacter->GetPlayerState()) };
	if (!ensureAlways(IsValid(PlayerState)))
	{
		return;
	}

	UNetherCrownPlayerStatComponent* PlayerStatComponent{ PlayerState->GetNetherCrownPlayerStatComponent() };
	check(PlayerStatComponent);

	PlayerStatComponent->ClearPlayerShield();
}
