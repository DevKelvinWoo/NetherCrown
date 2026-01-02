// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownCrowdControlComponent.h"

#include "Net/UnrealNetwork.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"

UNetherCrownCrowdControlComponent::UNetherCrownCrowdControlComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void UNetherCrownCrowdControlComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UNetherCrownCrowdControlComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UNetherCrownCrowdControlComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CrowdControlType);
}

void UNetherCrownCrowdControlComponent::Multicast_PlayCrowdControlAnim_Implementation(const ENetherCrownCrowdControlType InCrowdControlType)
{
	PlayCrowdControlAnim(InCrowdControlType);
}

void UNetherCrownCrowdControlComponent::ApplyCrowdControl(const ENetherCrownCrowdControlType InCrowdControlType, float DurationTime)
{
	//@NOTE : Only called by server
	CrowdControlType = InCrowdControlType;

	Multicast_PlayCrowdControlAnim(InCrowdControlType);

	UWorld* World{ GetWorld() };
	check(World);

	World->GetTimerManager().SetTimer(CrowdControlTimerHandle, this, &ThisClass::ClearCrowdControl, DurationTime, false);
}

void UNetherCrownCrowdControlComponent::KnockBack(const FVector& KnockBackVector) const
{
	ACharacter* OwnerCharacter{ Cast<ACharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)))
	{
		return;
	}

	OwnerCharacter->LaunchCharacter(KnockBackVector, true, true);
}

void UNetherCrownCrowdControlComponent::ClearCrowdControl()
{
	CrowdControlType = ENetherCrownCrowdControlType::NONE;
}

void UNetherCrownCrowdControlComponent::PlayCrowdControlAnim(const ENetherCrownCrowdControlType InCrowdControlType)
{
	if (InCrowdControlType == ENetherCrownCrowdControlType::NONE)
	{
		return;
	}

	if (CrowdControlAnimMap.IsEmpty())
	{
		return;
	}

	TSoftObjectPtr<UAnimMontage>* CrowdControlAnimMontageSoft{ CrowdControlAnimMap.Find(InCrowdControlType) };
	UAnimMontage* FoundCrowdControlAnimMontage{ CrowdControlAnimMontageSoft->LoadSynchronous() };
	if (!ensureAlways(IsValid(FoundCrowdControlAnimMontage)))
	{
		return;
	}

	ANetherCrownEnemy* OwnerEnemy{ Cast<ANetherCrownEnemy>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerEnemy)))
	{
		return;
	}

	OwnerEnemy->PlayAnimMontage(FoundCrowdControlAnimMontage);
}
