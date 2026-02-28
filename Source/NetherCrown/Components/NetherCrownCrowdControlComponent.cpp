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

	InitLoadData();
}

void UNetherCrownCrowdControlComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CrowdControlType);
}

void UNetherCrownCrowdControlComponent::InitLoadData()
{
	ACharacter* OwnerCharacter{ Cast<ACharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)) || OwnerCharacter->HasAuthority())
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
	PlayCrowdControlAnim(InCrowdControlType);
}

void UNetherCrownCrowdControlComponent::ApplyCrowdControl(const ENetherCrownCrowdControlType InCrowdControlType, float DurationTime)
{
	//@NOTE : Only called by server
	CrowdControlType = InCrowdControlType;

	const UWorld* World{ GetWorld() };
	check(World);

	//MultiCast로 CC Implements를 호출해버리기
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

void UNetherCrownCrowdControlComponent::Frozen() const
{
	ACharacter* OwnerCharacter{ Cast<ACharacter>(GetOwner()) };
	USkeletalMeshComponent* SkeletalMeshComponent{ OwnerCharacter ? OwnerCharacter->GetMesh() : nullptr};
	UCharacterMovementComponent* MovementComponent{ OwnerCharacter ? OwnerCharacter->GetCharacterMovement() : nullptr};
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

	MovementComponent->DisableMovement();
	SkeletalMeshComponent->bPauseAnims = true;
	SkeletalMeshComponent->SetOverlayMaterial(DMI);

	const UNetherCrownDefaultSettings* DefaultSettings{ GetDefault<UNetherCrownDefaultSettings>() };
	check(DefaultSettings);

	DMI->SetScalarParameterValue(DefaultSettings->FrozenTempestTargetMaterialParam, DefaultSettings->FrozenTempestTargetMaterialAlpha);
}

void UNetherCrownCrowdControlComponent::Stun() const
{
	const ACharacter* OwnerCharacter{ Cast<ACharacter>(GetOwner()) };
	UCharacterMovementComponent* OwnerCharacterMovementComponent{ OwnerCharacter ? OwnerCharacter->GetCharacterMovement() : nullptr};
	check(OwnerCharacterMovementComponent);

	OwnerCharacterMovementComponent->DisableMovement();

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
	ACharacter* OwnerCharacter{ Cast<ACharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)))
	{
		return;
	}

	UCharacterMovementComponent* MovementComponent{ OwnerCharacter->GetCharacterMovement() };
	check(MovementComponent);
	MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);

	USkeletalMeshComponent* SkeletalMeshComponent{ OwnerCharacter ? OwnerCharacter->GetMesh() : nullptr};
	if (IsValid(SkeletalMeshComponent))
	{
		SkeletalMeshComponent->bPauseAnims = false;
	}
}

void UNetherCrownCrowdControlComponent::ClearFrozenCosmetics()
{
	ACharacter* OwnerCharacter{ Cast<ACharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)))
	{
		return;
	}

	if (!OwnerCharacter->HasAuthority())
	{
		StartSetFrozenTargetOverlayEndMaterialTimeline();
	}
}

void UNetherCrownCrowdControlComponent::ClearStunCosmetics()
{
	Multicast_SetActiveStatusNiagaraSystem(ENetherCrownCrowdControlType::STUN, false);
}

void UNetherCrownCrowdControlComponent::Multicast_SetActiveStatusNiagaraSystem_Implementation(const ENetherCrownCrowdControlType InCrowdControlType, const bool bEnable) const
{
	const ACharacter* OwnerCharacter{ Cast<ACharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)) || OwnerCharacter->HasAuthority())
	{
		return;
	}

	const INetherCrownCrowdControlInterface* CrowdControlInterface{ Cast<INetherCrownCrowdControlInterface>(OwnerCharacter) };
	if (!ensureAlways(CrowdControlInterface))
	{
		return;
	}

	UNetherCrownStatusEffectControlComponent* StatusEffectControlComponent{ CrowdControlInterface->GetStatusEffectControlComponent() };
	if (!ensureAlways(StatusEffectControlComponent))
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
	FOnTimelineFloat FrozenTargetOverlayEndMaterialProgressFunc{};
	FrozenTargetOverlayEndMaterialProgressFunc.BindUFunction(this, FName("SetFrozenTargetOverlayEndMaterialByFloatTimeline"));
	FrozenTargetOverlayEndMaterialFloatTimeline.AddInterpFloat(CachedFrozenTargetOverlayMaterialEndCurveFloat, FrozenTargetOverlayEndMaterialProgressFunc);
}

void UNetherCrownCrowdControlComponent::StartSetFrozenTargetOverlayEndMaterialTimeline()
{
	FrozenTargetOverlayEndMaterialFloatTimeline.PlayFromStart();
}

void UNetherCrownCrowdControlComponent::SetFrozenTargetOverlayEndMaterialByFloatTimeline(float FloatCurveValue)
{
	ACharacter* OwnerCharacter{ Cast<ACharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)))
	{
		return;
	}

	USkeletalMeshComponent* MeshComponent{ OwnerCharacter->GetMesh() };
	check(MeshComponent);

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
