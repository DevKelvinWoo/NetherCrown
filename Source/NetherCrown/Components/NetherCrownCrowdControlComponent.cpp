// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownCrowdControlComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
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
	//@TODO : switch-case로 변경 후 각각을 함수로 로직을 빼서 호출하는 방식으로 변경
	if (CrowdControlType == ENetherCrownCrowdControlType::FROZEN)
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

		UCharacterMovementComponent* MovementComponent{ OwnerCharacter->GetCharacterMovement() };
		check(MovementComponent);
		MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);

		USkeletalMeshComponent* SkeletalMeshComponent{ OwnerCharacter ? OwnerCharacter->GetMesh() : nullptr};
		if (IsValid(SkeletalMeshComponent))
		{
			SkeletalMeshComponent->bPauseAnims = false;
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

void UNetherCrownCrowdControlComponent::StartFrozenTargetOverlayMaterialEndTimer()
{
	const UWorld* World{ GetWorld() };
	check(World);

	FrozenTargetOverlayMaterialElapsedTime = 0.f;

	World->GetTimerManager().SetTimer(FrozenTargetOverlayMaterialEndTimerHandle, this, &ThisClass::ApplyFrozenTargetOverlayEndMaterial, 0.01f, true, 0.f);
}

void UNetherCrownCrowdControlComponent::ApplyFrozenTargetOverlayEndMaterial()
{
	const UWorld* World{ GetWorld() };
	check(World);

	if (!ensureAlways(IsValid(CachedFrozenTargetOverlayMaterialEndCurveFloat)))
	{
		World->GetTimerManager().ClearTimer(FrozenTargetOverlayMaterialEndTimerHandle);
		return;
	}

	FrozenTargetOverlayMaterialElapsedTime += 0.01f;

	float Min{};
	float Max{};
	CachedFrozenTargetOverlayMaterialEndCurveFloat->GetTimeRange(Min, Max);

	if (FrozenTargetOverlayMaterialElapsedTime > Max)
	{
		World->GetTimerManager().ClearTimer(FrozenTargetOverlayMaterialEndTimerHandle);
		return;
	}

	ACharacter* OwnerCharacter{ Cast<ACharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)))
	{
		World->GetTimerManager().ClearTimer(FrozenTargetOverlayMaterialEndTimerHandle);
		return;
	}

	USkeletalMeshComponent* MeshComponent{ OwnerCharacter->GetMesh() };
	check(MeshComponent);

	UMaterialInstanceDynamic* Dynm = Cast<UMaterialInstanceDynamic>(MeshComponent->GetOverlayMaterial());
	if (!IsValid(Dynm))
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyCharacterOverlayEndMaterial - Cannot cast dynamic material"));
		return;
	}

	MeshComponent->SetOverlayMaterial(Dynm);

	const UNetherCrownDefaultSettings* DefaultSettings{ GetDefault<UNetherCrownDefaultSettings>() };
	check(DefaultSettings);

	const float CharacterOverlayMaterialAlpha{ CachedFrozenTargetOverlayMaterialEndCurveFloat->GetFloatValue(FrozenTargetOverlayMaterialElapsedTime) };
	Dynm->SetScalarParameterValue(DefaultSettings->FrozenTempestTargetMaterialParam, CharacterOverlayMaterialAlpha);
}
