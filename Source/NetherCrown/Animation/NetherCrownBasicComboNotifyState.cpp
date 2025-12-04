// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownBasicComboNotifyState.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownBasicAttackComponent.h"

void UNetherCrownBasicComboNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(MeshComp ? MeshComp->GetOwner() : nullptr)};
	UNetherCrownBasicAttackComponent* BasicAttackComponent{ OwnerCharacter ? OwnerCharacter->GetBasicAttackComponent() : nullptr };
	if (!(IsValid(BasicAttackComponent)))
	{
		return;
	}

	if (!OwnerCharacter->HasAuthority())
	{
		return;
	}

	BasicAttackComponent->EnableComboWindow();
}

void UNetherCrownBasicComboNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(MeshComp ? MeshComp->GetOwner() : nullptr) };
	UNetherCrownBasicAttackComponent* BasicAttackComponent{ OwnerCharacter ? OwnerCharacter->GetBasicAttackComponent() : nullptr };
	if (!(IsValid(BasicAttackComponent)))
	{
		return;
	}

	if (!OwnerCharacter->HasAuthority())
	{
		return;
	}

	BasicAttackComponent->DisableComboAndPlayQueuedComboWindow();
}
