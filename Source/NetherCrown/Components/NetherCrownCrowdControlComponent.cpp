// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownCrowdControlComponent.h"

#include "Net/UnrealNetwork.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Settings/NetherCrownDefaultSettings.h"

UNetherCrownCrowdControlComponent::UNetherCrownCrowdControlComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UNetherCrownCrowdControlComponent::BeginPlay()
{
	Super::BeginPlay();
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

	const UWorld* World{ GetWorld() };
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

void UNetherCrownCrowdControlComponent::Frozen(int32 DurationTime) const
{
	ACharacter* OwnerCharacter{ Cast<ACharacter>(GetOwner()) };
	USkeletalMeshComponent* SkeletalMeshComponent{ OwnerCharacter ? OwnerCharacter->GetMesh() : nullptr};
	UMaterialInterface* MaterialInterface{ SkeletalMeshComponent ? SkeletalMeshComponent->GetOverlayMaterial() : nullptr };
	if (!ensureAlways(IsValid(MaterialInterface)))
	{
		return;
	}

	UMaterialInstanceDynamic* DMI{ UMaterialInstanceDynamic::Create(MaterialInterface, OwnerCharacter) };
	if (!ensureAlways(IsValid(DMI)))
	{
		return;
	}

	SkeletalMeshComponent->bPauseAnims = true;
	SkeletalMeshComponent->SetOverlayMaterial(DMI);

	const UNetherCrownDefaultSettings* DefaultSettings{ GetDefault<UNetherCrownDefaultSettings>() };
	check(DefaultSettings);

	DMI->SetScalarParameterValue(DefaultSettings->FrozenTempestTargetMaterialParam, DefaultSettings->FrozenTempestTargetMaterialAlpha);
}

void UNetherCrownCrowdControlComponent::Multicast_ClearCrowdControl_Cosmetics_Implementation()
{
	if (CrowdControlType == ENetherCrownCrowdControlType::FROZEN)
	{
		ACharacter* OwnerCharacter{ Cast<ACharacter>(GetOwner()) };
		USkeletalMeshComponent* SkeletalMeshComponent{ OwnerCharacter ? OwnerCharacter->GetMesh() : nullptr};
		if (IsValid(SkeletalMeshComponent))
		{
			SkeletalMeshComponent->bPauseAnims = false;
			SkeletalMeshComponent->SetOverlayMaterial(nullptr);
		}
	}
}

void UNetherCrownCrowdControlComponent::ClearCrowdControl()
{
	Multicast_ClearCrowdControl_Cosmetics();

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
	if (!CrowdControlAnimMontageSoft)
	{
		return;
	}

	UAnimMontage* FoundCrowdControlAnimMontage{ CrowdControlAnimMontageSoft->LoadSynchronous() };
	if (!(IsValid(FoundCrowdControlAnimMontage)))
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
