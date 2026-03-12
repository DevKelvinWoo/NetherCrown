// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownCrowdControlComponent.h"

#include "NetherCrown/NetherCrown.h"
#include "NetherCrownStatusEffectControlComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Settings/NetherCrownDefaultSettings.h"

UNetherCrownCrowdControlComponent::UNetherCrownCrowdControlComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void UNetherCrownCrowdControlComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedOwner = Cast<ACharacter>(GetOwner());

	InitLoadData();
}

void UNetherCrownCrowdControlComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CrowdControlType);
}

void UNetherCrownCrowdControlComponent::InitLoadData()
{
	if (!ensureAlways(IsValid(CachedOwner)) || CachedOwner->HasAuthority())
	{
		return;
	}

	const UNetherCrownDefaultSettings* DefaultSettings{ GetDefault<UNetherCrownDefaultSettings>() };
	check(DefaultSettings);

	CachedFrozenTargetOverlayMaterialEndCurveFloat = DefaultSettings->TargetOverlayMaterialEndCurveFloatSoft.LoadSynchronous();

	BindTimelineFunctions();
}

void UNetherCrownCrowdControlComponent::Multicast_PlayCrowdControlAnim_Implementation(const ENetherCrownCrowdControlType InCrowdControlType)
{
	if (!ensureAlways(IsValid(CachedOwner)) || CachedOwner->HasAuthority())
	{
		return;
	}

	PlayCrowdControlAnim(InCrowdControlType);
}

void UNetherCrownCrowdControlComponent::ApplyCrowdControl(const ENetherCrownCrowdControlType InCrowdControlType, float DurationTime)
{
	if (!ensureAlways(IsValid(CachedOwner)) || !CachedOwner->HasAuthority())
	{
		return;
	}

	CrowdControlType = InCrowdControlType;

	const UWorld* World{ GetWorld() };
	check(World);

	World->GetTimerManager().SetTimer(CrowdControlTimerHandle, this, &ThisClass::ClearCrowdControl, DurationTime, false);
}

void UNetherCrownCrowdControlComponent::KnockBack(const FVector& KnockBackVector) const
{
	if (!ensureAlways(IsValid(CachedOwner)) || !CachedOwner->HasAuthority())
	{
		return;
	}

	CachedOwner->LaunchCharacter(KnockBackVector, true, true);
}

void UNetherCrownCrowdControlComponent::Frozen() const
{
	if (!ensureAlways(IsValid(CachedOwner)))
	{
		return;
	}

	UCharacterMovementComponent* MovementComponent{ CachedOwner->GetCharacterMovement() };
	if (!ensureAlways(IsValid(MovementComponent)))
	{
		return;
	}

	MovementComponent->DisableMovement();

	USkeletalMeshComponent* SkeletalMeshComponent{ CachedOwner->GetMesh() };
	UMaterialInterface* MaterialInterface{ SkeletalMeshComponent ? SkeletalMeshComponent->GetOverlayMaterial() : nullptr };
	if (!ensureAlways(IsValid(MaterialInterface)))
	{
		return;
	}

	UMaterialInstanceDynamic* DMI{ UMaterialInstanceDynamic::Create(MaterialInterface, CachedOwner) };
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

void UNetherCrownCrowdControlComponent::Stun() const
{
	if (!ensureAlways(IsValid(CachedOwner)) || !CachedOwner->HasAuthority())
	{
		return;
	}

	UCharacterMovementComponent* OwnerCharacterMovementComponent{ CachedOwner->GetCharacterMovement() };
	if (ensureAlways(IsValid(OwnerCharacterMovementComponent)))
	{
		OwnerCharacterMovementComponent->DisableMovement();
	}

	Multicast_SetActiveStatusNiagaraSystem(ENetherCrownCrowdControlType::STUN, true);
}

void UNetherCrownCrowdControlComponent::Multicast_ClearCrowdControl_Cosmetics_Implementation()
{
	switch (CrowdControlType)
	{
	case ENetherCrownCrowdControlType::FROZEN:
		ClearFrozenCosmetics();
		ResetMovementAndAnimationSettings();
		break;
	case ENetherCrownCrowdControlType::STUN:
		ClearStunCosmetics();
		break;
	default:
		break;
	}
}

void UNetherCrownCrowdControlComponent::ResetMovementAndAnimationSettings() const
{
	if (!ensureAlways(IsValid(CachedOwner)))
	{
		return;
	}

	if (CachedOwner->HasAuthority())
	{
		UCharacterMovementComponent* MovementComponent{ CachedOwner->GetCharacterMovement() };
		check(MovementComponent);
		MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
	}
	else
	{
		USkeletalMeshComponent* SkeletalMeshComponent{ CachedOwner->GetMesh() };
		if (IsValid(SkeletalMeshComponent))
		{
			SkeletalMeshComponent->bPauseAnims = false;
		}
	}
}

void UNetherCrownCrowdControlComponent::ClearFrozenCosmetics()
{
	if (!ensureAlways(IsValid(CachedOwner)) || CachedOwner->HasAuthority())
	{
		return;
	}

	StartSetFrozenTargetOverlayEndMaterialTimeline();
}

void UNetherCrownCrowdControlComponent::ClearStunCosmetics()
{
	Multicast_SetActiveStatusNiagaraSystem(ENetherCrownCrowdControlType::STUN, false);
}

void UNetherCrownCrowdControlComponent::Multicast_SetActiveStatusNiagaraSystem_Implementation(const ENetherCrownCrowdControlType InCrowdControlType, const bool bEnable) const
{
	if (!ensureAlways(IsValid(CachedOwner)) || CachedOwner->HasAuthority())
	{
		return;
	}

	const INetherCrownCrowdControlInterface* CrowdControlInterface{ Cast<INetherCrownCrowdControlInterface>(CachedOwner) };
	if (!ensureAlways(CrowdControlInterface))
	{
		return;
	}

	UNetherCrownStatusEffectControlComponent* StatusEffectControlComponent{ CrowdControlInterface->GetStatusEffectControlComponent() };
	if (!ensureAlways(IsValid(StatusEffectControlComponent)))
	{
		return;
	}

	StatusEffectControlComponent->SetActiveStatusNiagaraSystem(InCrowdControlType, bEnable);
}

void UNetherCrownCrowdControlComponent::ClearCrowdControl()
{
	Multicast_ClearCrowdControl_Cosmetics();

	CrowdControlType = ENetherCrownCrowdControlType::NONE;
}

void UNetherCrownCrowdControlComponent::PlayCrowdControlAnim(const ENetherCrownCrowdControlType InCrowdControlType)
{
	if (!ensureAlways(IsValid(CachedOwner)) || CachedOwner->HasAuthority())
	{
		return;
	}

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

	CachedOwner->PlayAnimMontage(FoundCrowdControlAnimMontage);
}

void UNetherCrownCrowdControlComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (FrozenTargetOverlayEndMaterialFloatTimeline.IsPlaying())
	{
		FrozenTargetOverlayEndMaterialFloatTimeline.TickTimeline(DeltaTime);
	}
}

void UNetherCrownCrowdControlComponent::BindTimelineFunctions()
{
	if (!ensureAlways(IsValid(CachedOwner)) || CachedOwner->HasAuthority())
	{
		return;
	}

	FOnTimelineFloat FrozenTargetOverlayEndMaterialProgressFunc{};
	FrozenTargetOverlayEndMaterialProgressFunc.BindUFunction(this, FName("SetFrozenTargetOverlayEndMaterialByFloatTimeline"));
	FrozenTargetOverlayEndMaterialFloatTimeline.AddInterpFloat(CachedFrozenTargetOverlayMaterialEndCurveFloat, FrozenTargetOverlayEndMaterialProgressFunc);
}

void UNetherCrownCrowdControlComponent::StartSetFrozenTargetOverlayEndMaterialTimeline()
{
	if (!ensureAlways(IsValid(CachedOwner)) || CachedOwner->HasAuthority())
	{
		return;
	}

	FrozenTargetOverlayEndMaterialFloatTimeline.PlayFromStart();
}

void UNetherCrownCrowdControlComponent::SetFrozenTargetOverlayEndMaterialByFloatTimeline(float FloatCurveValue)
{
	if (!ensureAlways(IsValid(CachedOwner)) || CachedOwner->HasAuthority())
	{
		return;
	}

	USkeletalMeshComponent* MeshComponent{ CachedOwner->GetMesh() };
	if (!ensureAlways(IsValid(MeshComponent)))
	{
		return;
	}

	UMaterialInstanceDynamic* DynamicOverlayMaterial = Cast<UMaterialInstanceDynamic>(MeshComponent->GetOverlayMaterial());
	if (!IsValid(DynamicOverlayMaterial))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("ApplyCharacterOverlayEndMaterial - Cannot cast dynamic material"));
		return;
	}

	MeshComponent->SetOverlayMaterial(DynamicOverlayMaterial);

	const UNetherCrownDefaultSettings* DefaultSettings{ GetDefault<UNetherCrownDefaultSettings>() };
	check(DefaultSettings);

	DynamicOverlayMaterial->SetScalarParameterValue(DefaultSettings->FrozenTempestTargetMaterialParam, FloatCurveValue);
}
