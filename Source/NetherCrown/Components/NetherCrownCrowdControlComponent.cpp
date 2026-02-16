// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownCrowdControlComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Settings/NetherCrownDefaultSettings.h"
#include "NetherCrown/Util/NetherCrownCurveTimerUtil.h"

UNetherCrownCrowdControlComponent::UNetherCrownCrowdControlComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

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

	//@TODO : Stun Effect 출력 필요
}

void UNetherCrownCrowdControlComponent::Multicast_ClearCrowdControl_Cosmetics_Implementation()
{
	switch (CrowdControlType)
	{
	case ENetherCrownCrowdControlType::FROZEN:
		ClearFrozenCosmetics();
		ResetMovementAndAnimation();
		break;
	case ENetherCrownCrowdControlType::STUN:
		ClearStunCosmetics();
		break;
	default:
		break;
	}
}

void UNetherCrownCrowdControlComponent::ResetMovementAndAnimation() const
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
		StartFrozenTargetOverlayMaterialEndTimer();
	}
}

void UNetherCrownCrowdControlComponent::ClearStunCosmetics()
{
	//@TODO : Stun Icon 해제 및 다른 Cosmetic 해제 로직 구현
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

void UNetherCrownCrowdControlComponent::StartFrozenTargetOverlayMaterialEndTimer()
{
	const UWorld* World{ GetWorld() };
	check(World);

	FrozenTargetOverlayMaterialElapsedTime = 0.f;

	World->GetTimerManager().SetTimer(FrozenTargetOverlayMaterialEndTimerHandle, this, &ThisClass::ApplyFrozenTargetOverlayEndMaterial, 0.01f, true, 0.f);
}

void UNetherCrownCrowdControlComponent::ApplyFrozenTargetOverlayEndMaterial()
{
	FNetherCrownCurveTimerData CurveTimerData{};
	CurveTimerData.WorldContextObject = this;
	CurveTimerData.Curve = CachedFrozenTargetOverlayMaterialEndCurveFloat;
	CurveTimerData.TimerHandle = &FrozenTargetOverlayMaterialEndTimerHandle;
	CurveTimerData.CurveElapsedTime = &FrozenTargetOverlayMaterialElapsedTime;
	CurveTimerData.CurveElapsedTimeOffset = 0.01f;
	CurveTimerData.CallBack = [WeakThis = MakeWeakObjectPtr(this)]() { WeakThis->SetEndFrozenTargetOverlayMaterialScalarParam(); };

	FNetherCrownCurveTimerUtil::ExecuteLoopTimerCallbackByCurve(CurveTimerData);
}

void UNetherCrownCrowdControlComponent::SetEndFrozenTargetOverlayMaterialScalarParam()
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
		UE_LOG(LogTemp, Warning, TEXT("ApplyCharacterOverlayEndMaterial - Cannot cast dynamic material"));
		return;
	}

	MeshComponent->SetOverlayMaterial(DynamicOverlayMaterial);

	const UNetherCrownDefaultSettings* DefaultSettings{ GetDefault<UNetherCrownDefaultSettings>() };
	check(DefaultSettings);

	const float CharacterOverlayMaterialAlpha{ CachedFrozenTargetOverlayMaterialEndCurveFloat->GetFloatValue(FrozenTargetOverlayMaterialElapsedTime) };
	DynamicOverlayMaterial->SetScalarParameterValue(DefaultSettings->FrozenTempestTargetMaterialParam, CharacterOverlayMaterialAlpha);
}
