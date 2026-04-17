// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownCrowdControlComponent.h"

#include "NetherCrown/NetherCrown.h"
#include "NetherCrownStatusEffectControlComponent.h"
#include "Animation/AnimMontage.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/CharacterMovementComponent.h"
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
	if (!ensureAlways(IsValid(CachedOwner)) || CachedOwner->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	LoadCrowdControlCosmeticData();
	BindTimelineFunctions();
}

void UNetherCrownCrowdControlComponent::LoadCrowdControlCosmeticData()
{
	if (CrowdControlCosmeticDataAssetSoft.IsNull() || CachedOwner->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	const UNetherCrownCrowdControlCosmeticDataAsset* CrowdControlCosmeticDataAsset{ CrowdControlCosmeticDataAssetSoft.LoadSynchronous() };
	if (!ensureAlways(IsValid(CrowdControlCosmeticDataAsset)))
	{
		return;
	}

	CrowdControlCosmeticData = CrowdControlCosmeticDataAsset->GetCrowdControlCosmeticData();

	if (!CrowdControlCosmeticData.FrozenCosmeticData.OverlayEndCurveFloatSoft.IsNull())
	{
		CachedFrozenTargetOverlayMaterialEndCurveFloat = CrowdControlCosmeticData.FrozenCosmeticData.OverlayEndCurveFloatSoft.LoadSynchronous();
	}
}

void UNetherCrownCrowdControlComponent::Multicast_SetCrowdControlState_Implementation(const ENetherCrownCrowdControlType InCrowdControlType)
{
	if (!ensureAlways(IsValid(CachedOwner)))
	{
		return;
	}

	SetCrowdControlActive(InCrowdControlType, true);
	RefreshCrowdControlType();
	RefreshMovementAndAnimationSettings();
}

void UNetherCrownCrowdControlComponent::ApplyCrowdControl(const ENetherCrownCrowdControlType InCrowdControlType, float DurationTime)
{
	if (!ensureAlways(IsValid(CachedOwner)) || !CachedOwner->HasAuthority())
	{
		return;
	}

	SetCrowdControlActive(InCrowdControlType, true);
	RefreshCrowdControlType();

	const UWorld* World{ GetWorld() };
	check(World);

	FTimerHandle* CrowdControlTimerHandle{ GetCrowdControlTimerHandle(InCrowdControlType) };
	if (!ensureAlways(CrowdControlTimerHandle))
	{
		return;
	}

	FTimerDelegate CrowdControlTimerDelegate{};
	CrowdControlTimerDelegate.BindUObject(this, &ThisClass::ClearCrowdControl, InCrowdControlType);
	World->GetTimerManager().SetTimer(*CrowdControlTimerHandle, CrowdControlTimerDelegate, DurationTime, false);

	Multicast_SetCrowdControlState(InCrowdControlType);
}

void UNetherCrownCrowdControlComponent::KnockBack(const FVector& KnockBackVector) const
{
	if (!ensureAlways(IsValid(CachedOwner)) || !CachedOwner->HasAuthority())
	{
		return;
	}

	UCharacterMovementComponent* MovementComponent{ CachedOwner->GetCharacterMovement() };
	if (!ensureAlways(IsValid(MovementComponent)))
	{
		return;
	}

	MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
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

	if (CachedOwner->HasAuthority())
	{
		MovementComponent->DisableMovement();
	}

	if (CachedOwner->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

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
	DMI->SetScalarParameterValue(CrowdControlCosmeticData.FrozenCosmeticData.OverlayMaterialParam, CrowdControlCosmeticData.FrozenCosmeticData.OverlayAlpha);
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

	Multicast_SetActiveStatusNiagaraSystem(true, ENetherCrownCrowdControlType::STUN);
}

bool UNetherCrownCrowdControlComponent::IsCrowdControlActive(const ENetherCrownCrowdControlType InCrowdControlType) const
{
	if (InCrowdControlType == ENetherCrownCrowdControlType::NONE)
	{
		return false;
	}

	const uint8 BitMask{ static_cast<uint8>(1 << static_cast<uint8>(InCrowdControlType)) };
	return (ActiveCrowdControlFlags & BitMask) != 0;
}

void UNetherCrownCrowdControlComponent::Multicast_ClearCrowdControl_Cosmetics_Implementation(const ENetherCrownCrowdControlType InCrowdControlType)
{
	if (!ensureAlways(IsValid(CachedOwner)))
	{
		return;
	}

	SetCrowdControlActive(InCrowdControlType, false);

	switch (InCrowdControlType)
	{
	case ENetherCrownCrowdControlType::FROZEN:
		ClearFrozenCosmetics();
		break;
	case ENetherCrownCrowdControlType::STUN:
		ClearStunCosmetics();
		break;
	default:
		break;
	}

	RefreshCrowdControlType();
	RefreshMovementAndAnimationSettings();
}

void UNetherCrownCrowdControlComponent::RefreshCrowdControlType()
{
	CrowdControlType = GetHighestPriorityCrowdControlType();
}

void UNetherCrownCrowdControlComponent::RefreshMovementAndAnimationSettings() const
{
	if (!ensureAlways(IsValid(CachedOwner)))
	{
		return;
	}

	if (CachedOwner->HasAuthority())
	{
		UCharacterMovementComponent* MovementComponent{ CachedOwner->GetCharacterMovement() };
		if (!ensureAlways(IsValid(MovementComponent)))
		{
			return;
		}

		const bool bHasBlockingMovementCrowdControl{ IsCrowdControlActive(ENetherCrownCrowdControlType::FROZEN) || IsCrowdControlActive(ENetherCrownCrowdControlType::STUN) };
		if (bHasBlockingMovementCrowdControl && !IsCrowdControlActive(ENetherCrownCrowdControlType::KNOCK_BACK))
		{
			MovementComponent->DisableMovement();
		}
		else
		{
			MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
		}
	}

	if (CachedOwner->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	USkeletalMeshComponent* SkeletalMeshComponent{ CachedOwner->GetMesh() };
	if (!ensureAlways(IsValid(SkeletalMeshComponent)))
	{
		return;
	}

	SkeletalMeshComponent->bPauseAnims = IsCrowdControlActive(ENetherCrownCrowdControlType::FROZEN);
}

void UNetherCrownCrowdControlComponent::SetCrowdControlActive(const ENetherCrownCrowdControlType InCrowdControlType, const bool bActive)
{
	if (InCrowdControlType == ENetherCrownCrowdControlType::NONE)
	{
		return;
	}

	const uint8 BitMask{ static_cast<uint8>(1 << static_cast<uint8>(InCrowdControlType)) };
	if (bActive)
	{
		ActiveCrowdControlFlags |= BitMask;
		return;
	}

	ActiveCrowdControlFlags &= ~BitMask;
}

FTimerHandle* UNetherCrownCrowdControlComponent::GetCrowdControlTimerHandle(const ENetherCrownCrowdControlType InCrowdControlType)
{
	switch (InCrowdControlType)
	{
	case ENetherCrownCrowdControlType::KNOCK_BACK:
		return &KnockBackCrowdControlTimerHandle;
	case ENetherCrownCrowdControlType::FROZEN:
		return &FrozenCrowdControlTimerHandle;
	case ENetherCrownCrowdControlType::STUN:
		return &StunCrowdControlTimerHandle;
	default:
		return nullptr;
	}
}

ENetherCrownCrowdControlType UNetherCrownCrowdControlComponent::GetHighestPriorityCrowdControlType() const
{
	ENetherCrownCrowdControlType HighestPriorityCrowdControlType{ ENetherCrownCrowdControlType::NONE };
	int32 HighestPriority{ GetCrowdControlPriority(HighestPriorityCrowdControlType) };

	for (const ENetherCrownCrowdControlType CandidateCrowdControlType : { ENetherCrownCrowdControlType::KNOCK_BACK, ENetherCrownCrowdControlType::FROZEN, ENetherCrownCrowdControlType::STUN })
	{
		if (!IsCrowdControlActive(CandidateCrowdControlType))
		{
			continue;
		}

		const int32 CandidatePriority{ GetCrowdControlPriority(CandidateCrowdControlType) };
		if (CandidatePriority <= HighestPriority)
		{
			continue;
		}

		HighestPriority = CandidatePriority;
		HighestPriorityCrowdControlType = CandidateCrowdControlType;
	}

	return HighestPriorityCrowdControlType;
}

int32 UNetherCrownCrowdControlComponent::GetCrowdControlPriority(const ENetherCrownCrowdControlType InCrowdControlType) const
{
	//@NOTE : 겹치는 CC 중 대표 상태는 Frozen > Stun > KnockBack 순서로 유지한다.
	constexpr int32 FROZEN_PRIORITY{ 300 };
	constexpr int32 STUN_PRIORITY{ 200 };
	constexpr int32 KNOCK_BACK_PRIORITY{ 100 };

	switch (InCrowdControlType)
	{
	case ENetherCrownCrowdControlType::FROZEN:
		return FROZEN_PRIORITY;
	case ENetherCrownCrowdControlType::STUN:
		return STUN_PRIORITY;
	case ENetherCrownCrowdControlType::KNOCK_BACK:
		return KNOCK_BACK_PRIORITY;
	default:
		return 0;
	}
}

void UNetherCrownCrowdControlComponent::ClearFrozenCosmetics()
{
	if (!ensureAlways(IsValid(CachedOwner)) || CachedOwner->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	StartSetFrozenTargetOverlayEndMaterialTimeline();
}

void UNetherCrownCrowdControlComponent::ClearStunCosmetics()
{
	Multicast_SetActiveStatusNiagaraSystem(false);
}

void UNetherCrownCrowdControlComponent::Multicast_SetActiveStatusNiagaraSystem_Implementation(const bool bEnable, const ENetherCrownCrowdControlType InCrowdControlType) const
{
	if (!ensureAlways(IsValid(CachedOwner)) || CachedOwner->GetNetMode() == NM_DedicatedServer)
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

	StatusEffectControlComponent->SetActiveStatusNiagaraSystem(bEnable, InCrowdControlType);
}

void UNetherCrownCrowdControlComponent::ClearCrowdControl(const ENetherCrownCrowdControlType InCrowdControlType)
{
	if (!ensureAlways(IsValid(CachedOwner)) || !CachedOwner->HasAuthority() || !IsCrowdControlActive(InCrowdControlType))
	{
		return;
	}

	UWorld* World{ GetWorld() };
	check(World);

	FTimerHandle* CrowdControlTimerHandle{ GetCrowdControlTimerHandle(InCrowdControlType) };
	if (CrowdControlTimerHandle)
	{
		World->GetTimerManager().ClearTimer(*CrowdControlTimerHandle);
	}

	SetCrowdControlActive(InCrowdControlType, false);
	RefreshCrowdControlType();

	Multicast_ClearCrowdControl_Cosmetics(InCrowdControlType);
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
	if (!ensureAlways(IsValid(CachedOwner)) || CachedOwner->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (!ensureAlways(IsValid(CachedFrozenTargetOverlayMaterialEndCurveFloat)))
	{
		return;
	}

	FOnTimelineFloat FrozenTargetOverlayEndMaterialProgressFunc{};
	FrozenTargetOverlayEndMaterialProgressFunc.BindUFunction(this, FName("SetFrozenTargetOverlayEndMaterialByFloatTimeline"));
	FrozenTargetOverlayEndMaterialFloatTimeline.AddInterpFloat(CachedFrozenTargetOverlayMaterialEndCurveFloat, FrozenTargetOverlayEndMaterialProgressFunc);
}

void UNetherCrownCrowdControlComponent::StartSetFrozenTargetOverlayEndMaterialTimeline()
{
	if (!ensureAlways(IsValid(CachedOwner)) || CachedOwner->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	FrozenTargetOverlayEndMaterialFloatTimeline.PlayFromStart();
}

void UNetherCrownCrowdControlComponent::SetFrozenTargetOverlayEndMaterialByFloatTimeline(float FloatCurveValue)
{
	if (!ensureAlways(IsValid(CachedOwner)) || CachedOwner->GetNetMode() == NM_DedicatedServer)
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
	DynamicOverlayMaterial->SetScalarParameterValue(CrowdControlCosmeticData.FrozenCosmeticData.OverlayMaterialParam, FloatCurveValue);
}
