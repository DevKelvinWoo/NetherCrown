// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownInteractComponent.h"

#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/NetherCrown.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Character/NetherCrownPlayerController.h"
#include "NetherCrown/Interact/NetherCrownInteract.h"
#include "NetherCrown/Interact/NetherCrownInteractNPC.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

UNetherCrownInteractComponent::UNetherCrownInteractComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UNetherCrownInteractComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UNetherCrownInteractComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TargetInteractActor);
}

void UNetherCrownInteractComponent::RestoreCameraPosition()
{
	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)) || GetNetMode() == NM_DedicatedServer || !OwnerCharacter->IsLocallyControlled())
	{
		return;
	}

	OwnerCharacter->SetMeshVisibility(false);

	auto* InteractActor{ Cast<INetherCrownInteract>(TargetInteractActor) };
	if (!InteractActor)
	{
		return;
	}

	if (InteractActor->IsNeedCameraMoving())
	{
		ANetherCrownPlayerController* PlayerController{ Cast<ANetherCrownPlayerController>(OwnerCharacter->GetController()) };
		if (IsValid(PlayerController) && IsValid(PreviousViewTarget))
		{
			PlayerController->SetViewTargetWithBlend(PreviousViewTarget, 1.f, VTBlend_EaseInOut, 2.f, true);
		}

		if (IsValid(InteractCameraActor))
		{
			InteractCameraActor->Destroy();
		}
	}
}

void UNetherCrownInteractComponent::Client_PlayInteractSound_Implementation()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	FNetherCrownUtilManager::PlaySound2DByGameplayTag(this, NetherCrownTags::Sound_Interact);
}

void UNetherCrownInteractComponent::Client_MoveCameraToInteractPosition_Implementation()
{
	if (!IsValid(TargetInteractActor))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("TargetInteractActor is invalid in %hs"), __FUNCTION__);
		return;
	}

	UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	FActorSpawnParameters SpawnParams{};
	SpawnParams.Owner = GetOwner();
	SpawnParams.ObjectFlags = RF_Transient;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector InteractCameraLocation{ TargetInteractActor->GetActorLocation() + (TargetInteractActor->GetActorForwardVector() * 200.0f) };
	ANetherCrownInteractNPC* InteractNPC{ Cast<ANetherCrownInteractNPC>(TargetInteractActor) };
	if (IsValid(InteractNPC))
	{
		InteractCameraLocation = InteractNPC->GetInteractCameraPos();
	}

	FRotator InteractCameraRotation{ UKismetMathLibrary::FindLookAtRotation(InteractCameraLocation, TargetInteractActor->GetActorLocation()) };
	if (IsValid(InteractNPC))
	{
		InteractCameraRotation = InteractNPC->GetInteractCameraRot();
	}

	InteractCameraActor = World->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), InteractCameraLocation, InteractCameraRotation, SpawnParams);
	if (!ensureAlways(IsValid(InteractCameraActor)))
	{
		return;
	}

	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)))
	{
		InteractCameraActor->Destroy();
		return;
	}

	OwnerCharacter->SetMeshVisibility(true);

	ANetherCrownPlayerController* PlayerController{ Cast<ANetherCrownPlayerController>(OwnerCharacter->GetController()) };
	if (!ensureAlways(IsValid(PlayerController)))
	{
		InteractCameraActor->Destroy();
		return;
	}

	PreviousViewTarget = PlayerController->GetViewTarget();
	if (!ensureAlways(IsValid(PreviousViewTarget)))
	{
		InteractCameraActor->Destroy();
		return;
	}

	UCameraComponent* InteractCameraComponent{ InteractCameraActor->GetCameraComponent() };
	if (!ensureAlways(IsValid(InteractCameraComponent)))
	{
		InteractCameraActor->Destroy();
		return;
	}

	APlayerCameraManager* CameraManager{ PlayerController->PlayerCameraManager };
	if (!ensureAlways(IsValid(CameraManager)))
	{
		InteractCameraActor->Destroy();
		return;
	}

	InteractCameraComponent->SetFieldOfView(CameraManager->GetFOVAngle());
	InteractCameraComponent->bConstrainAspectRatio = false;

	int32 ViewportSizeX{ 0 };
	int32 ViewportSizeY{ 0 };
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	if (ViewportSizeX > 0 && ViewportSizeY > 0)
	{
		InteractCameraComponent->SetAspectRatio(static_cast<float>(ViewportSizeX) / static_cast<float>(ViewportSizeY));
	}

	const float CameraBlendTime{ 1.f };
	const float CameraBlendExp{ 2.f };
	PlayerController->SetViewTargetWithBlend(InteractCameraActor, CameraBlendTime, VTBlend_Cubic, CameraBlendExp, true);
}

void UNetherCrownInteractComponent::Server_InteractToTarget_Implementation()
{
	auto* InteractActor{ Cast<INetherCrownInteract>(TargetInteractActor) };
	if (!InteractActor)
	{
		return;
	}

	Client_PlayInteractSound();

	if (InteractActor->IsNeedCameraMoving())
	{
		Client_MoveCameraToInteractPosition();
	}

	InteractActor->Interact();
}

void UNetherCrownInteractComponent::Server_FinishInteract_Implementation()
{
	auto* InteractActor{ Cast<INetherCrownInteract>(TargetInteractActor) };
	if (!InteractActor)
	{
		return;
	}

	ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)) || !OwnerCharacter->HasAuthority())
	{
		return;
	}

	InteractActor->FinishInteract(OwnerCharacter);
}

void UNetherCrownInteractComponent::SetTargetInteractActor(AActor* InTargetInteractActor)
{
	TargetInteractActor = InTargetInteractActor;
}

void UNetherCrownInteractComponent::InteractToTarget()
{
	Server_InteractToTarget();
}

void UNetherCrownInteractComponent::FinishInteract()
{
	RestoreCameraPosition();
	Server_FinishInteract();
}
