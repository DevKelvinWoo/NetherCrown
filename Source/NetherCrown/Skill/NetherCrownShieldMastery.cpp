// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownShieldMastery.h"

#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"
#include "NiagaraComponent.h"
#include "SkillActors/NetherCrownShield.h"

void UNetherCrownShieldMastery::PlaySkillCosmetics()
{
	Super::PlaySkillCosmetics();

	ActiveShieldEffectAndActor();
}

void UNetherCrownShieldMastery::ExecuteSkillGameplay()
{
	Super::ExecuteSkillGameplay();

	//@TODO : 실제 쉴드를 부여하는 로직 추가 필요
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

	HandledShieldMasteryActor->AttachToActor(SkillOwnerCharacter, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("root"));
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
