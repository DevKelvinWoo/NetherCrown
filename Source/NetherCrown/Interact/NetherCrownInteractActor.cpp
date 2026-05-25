// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownInteractActor.h"

#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Character/NetherCrownPlayerController.h"
#include "NetherCrown/Components/NetherCrownInteractComponent.h"
#include "NetherCrown/Data/NetherCrownInteractData.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/UI/NetherCrownUIManagerSubsystem.h"
#include "NetherCrown/Widgets/NetherCrownInteractWidgetView.h"
#include "NetherCrown/Widgets/NetherCrownNPCDialogueWidgetView.h"

ANetherCrownInteractActor::ANetherCrownInteractActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	InteractDetectSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("InteractDetectSphereComponent"));
	InteractDetectSphereComponent->SetupAttachment(RootComponent);

	InteractWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidgetComponent"));
	InteractWidgetComponent->SetupAttachment(RootComponent);
	InteractWidgetComponent->SetVisibility(false);

	InteractCameraPosSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("InteractCameraPosSphereComponent"));
	InteractCameraPosSphereComponent->SetupAttachment(RootComponent);

	bNetLoadOnClient = true;
	bReplicates = true;
}

FVector ANetherCrownInteractActor::GetInteractCameraPos() const
{
	if (!ensureAlways(IsValid(InteractCameraPosSphereComponent)))
	{
		return FVector::ZeroVector;
	}

	return InteractCameraPosSphereComponent->GetComponentLocation();
}

FRotator ANetherCrownInteractActor::GetInteractCameraRot() const
{
	if (!ensureAlways(IsValid(InteractCameraPosSphereComponent)))
	{
		return FRotator::ZeroRotator;
	}

	return InteractCameraPosSphereComponent->GetComponentRotation();
}

const FGameplayTag& ANetherCrownInteractActor::GetInteractActorTag() const
{
	if (!IsValid(CachedInteractDataAsset))
	{
		static const FGameplayTag EmptyTag{};
		return EmptyTag;
	}

	return CachedInteractDataAsset->GetInteractActorTag();
}

void ANetherCrownInteractActor::BeginPlay()
{
	Super::BeginPlay();

	CacheInteractDataAsset();

	if (ensureAlways(IsValid(InteractDetectSphereComponent)))
	{
		InteractDetectSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandleOnDetectSphereOverlapBegin);
		InteractDetectSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::HandleOnDetectSphereOverlapEnd);
	}
}

void ANetherCrownInteractActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InteractTargetCharacterWeak);
}

void ANetherCrownInteractActor::Interact()
{
	ANetherCrownCharacter* InteractTargetCharacter{ InteractTargetCharacterWeak.Get() };
	if (!IsValid(InteractTargetCharacter) || !HasAuthority())
	{
		return;
	}

	Multicast_SetInteractWidgetVisibility(InteractTargetCharacter, false);
	Multicast_ShowInteractActorDialogueWidget();
}

void ANetherCrownInteractActor::FinishInteract(ANetherCrownCharacter* InteractCharacter)
{
	if (!ensureAlways(IsValid(InteractCharacter)) || !InteractCharacter->HasAuthority())
	{
		return;
	}

	Multicast_SetInteractWidgetVisibility(InteractCharacter, false);
}

void ANetherCrownInteractActor::HandleOnDetectSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent,
                                                                 AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                                 const FHitResult& SweepResult)
{
	InteractTargetCharacterWeak = MakeWeakObjectPtr(Cast<ANetherCrownCharacter>(OtherActor));
	ANetherCrownCharacter* InteractTargetCharacter{ InteractTargetCharacterWeak.Get() };
	if (!IsValid(InteractTargetCharacter))
	{
		return;
	}

	if (InteractTargetCharacter->IsLocallyControlled())
	{
		SetInteractWidgetVisibility(InteractTargetCharacter, true);
	}

	if (HasAuthority())
	{
		SetTargetInteractActor(InteractTargetCharacter, true);
	}
}

void ANetherCrownInteractActor::HandleOnDetectSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	InteractTargetCharacterWeak = MakeWeakObjectPtr(Cast<ANetherCrownCharacter>(OtherActor));
	ANetherCrownCharacter* InteractTargetCharacter{ InteractTargetCharacterWeak.Get() };
	if (!IsValid(InteractTargetCharacter))
	{
		return;
	}

	if (InteractTargetCharacter->IsLocallyControlled())
	{
		SetInteractWidgetVisibility(InteractTargetCharacter, false);
	}

	if (HasAuthority())
	{
		SetTargetInteractActor(InteractTargetCharacter, false);
	}
}

void ANetherCrownInteractActor::SetTargetInteractActor(const ANetherCrownCharacter* InteractCharacter, bool bTargetValid)
{
	if (!HasAuthority())
	{
		return;
	}

	UNetherCrownInteractComponent* InteractComponent{ InteractCharacter->GetInteractComponent() };
	if (!ensureAlways(IsValid(InteractComponent)))
	{
		return;
	}

	ANetherCrownInteractActor* InteractActor{ bTargetValid ? this : nullptr };
	InteractComponent->SetTargetInteractActor(InteractActor);
}

void ANetherCrownInteractActor::SetInteractWidgetVisibility(const ANetherCrownCharacter* InteractTarget, bool bVisible) const
{
	if (!InteractTarget->IsLocallyControlled())
	{
		return;
	}

	if (!ensureAlways(IsValid(InteractWidgetComponent)))
	{
		return;
	}

	if (bVisible)
	{
		UNetherCrownInteractWidgetView* InteractWidgetView{ Cast<UNetherCrownInteractWidgetView>(InteractWidgetComponent->GetWidget()) };
		if (ensureAlways(IsValid(InteractWidgetView)))
		{
			InteractWidgetView->SetInteractWidgetBorderTexture(CachedInteractWidgetTexture);
		}
	}

	InteractWidgetComponent->SetVisibility(bVisible);
}

void ANetherCrownInteractActor::ToggleInteractBoxCollision(const bool bOn)
{
	if (!HasAuthority())
	{
		return;
	}

	Multicast_SetInteractDetectSphereCollision(bOn);
}

void ANetherCrownInteractActor::Multicast_SetInteractDetectSphereCollision_Implementation(const bool bOn)
{
	if (!ensureAlways(IsValid(InteractDetectSphereComponent)))
	{
		return;
	}

	InteractDetectSphereComponent->SetGenerateOverlapEvents(bOn);
	InteractDetectSphereComponent->SetCollisionEnabled(bOn ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void ANetherCrownInteractActor::CacheInteractDataAsset()
{
	if (InteractDataAssetSoft.IsNull())
	{
		return;
	}

	CachedInteractDataAsset = InteractDataAssetSoft.LoadSynchronous();
	if (!ensureAlways(IsValid(CachedInteractDataAsset)))
	{
		return;
	}

	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	TSoftObjectPtr<UTexture2D> InteractWidgetTextureSoft{ CachedInteractDataAsset->GetInteractWidgetTexture() };
	if (!InteractWidgetTextureSoft.IsNull())
	{
		CachedInteractWidgetTexture = InteractWidgetTextureSoft.LoadSynchronous();
	}
}

void ANetherCrownInteractActor::Multicast_ShowInteractActorDialogueWidget_Implementation()
{
	ANetherCrownCharacter* InteractTargetCharacter{ InteractTargetCharacterWeak.Get() };
	if (!ensureAlways(IsValid(InteractTargetCharacter)) || !InteractTargetCharacter->IsLocallyControlled())
	{
		return;
	}

	const ANetherCrownPlayerController* PlayerController{ Cast<ANetherCrownPlayerController>(InteractTargetCharacter->GetController()) };
	if (!ensureAlways(IsValid(PlayerController)))
	{
		return;
	}

	const ULocalPlayer* LocalPlayer{ PlayerController->GetLocalPlayer() };
	if (!ensureAlways(IsValid(LocalPlayer)))
	{
		return;
	}

	UNetherCrownUIManagerSubsystem* UIManagerSubsystem{ LocalPlayer->GetSubsystem<UNetherCrownUIManagerSubsystem>() };
	if (!ensureAlways(IsValid(UIManagerSubsystem)))
	{
		return;
	}

	UNetherCrownNPCDialogueWidgetView* NPCDialogueWidget{ Cast<UNetherCrownNPCDialogueWidgetView>(UIManagerSubsystem->ShowScreenByTag(NetherCrownTags::UI_Screen_NPCDialogue)) };
	if (!ensureAlways(IsValid(NPCDialogueWidget)))
	{
		return;
	}

	if (!ensureAlways(IsValid(CachedInteractDataAsset)))
	{
		return;
	}

	NPCDialogueWidget->InitViewModel(InteractTargetCharacter);
	NPCDialogueWidget->SetDialogueText({ CachedInteractDataAsset->GetDialogueText() }, 0);
}

void ANetherCrownInteractActor::Multicast_SetInteractWidgetVisibility_Implementation(
	const ANetherCrownCharacter* InteractTarget, bool bVisible) const
{
	if (!ensureAlways(IsValid(InteractTarget)) || !InteractTarget->IsLocallyControlled())
	{
		return;
	}

	SetInteractWidgetVisibility(InteractTarget, bVisible);
}
