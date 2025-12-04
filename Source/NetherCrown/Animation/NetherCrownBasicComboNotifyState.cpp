// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownBasicComboNotifyState.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownBasicAttackComponent.h"

void UNetherCrownBasicComboNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	StartTime = MeshComp->GetWorld()->GetTimeSeconds();

	if (!(IsValid(MeshComp)))
	{
		return;
	}

	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(MeshComp->GetOwner() )};
	if (!(IsValid(OwnerCharacter)))
	{
		return;
	}

	UNetherCrownBasicAttackComponent* BasicAttackComponent{ OwnerCharacter->GetBasicAttackComponent() };
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
	float EndTime = MeshComp->GetWorld()->GetTimeSeconds();
	float Duration = EndTime - StartTime;

	UE_LOG(LogTemp, Warning, TEXT("Notify Duration: %f"), Duration);

	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!(IsValid(MeshComp)))
	{
		return;
	}

	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(MeshComp->GetOwner() )};
	if (!(IsValid(OwnerCharacter)))
	{
		return;
	}

	UNetherCrownBasicAttackComponent* BasicAttackComponent{ OwnerCharacter->GetBasicAttackComponent() };
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
