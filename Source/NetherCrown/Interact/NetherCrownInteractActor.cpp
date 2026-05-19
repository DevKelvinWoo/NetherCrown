// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownInteractActor.h"

#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownInteractComponent.h"
#include "NetherCrown/Widgets/NetherCrownInteractWidgetView.h"

ANetherCrownInteractActor::ANetherCrownInteractActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	InteractDetectSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("InteractDetectSphereComponent"));
	InteractDetectSphereComponent->SetupAttachment(RootComponent);

	InteractWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidgetComponent"));
	InteractWidgetComponent->SetupAttachment(RootComponent);
	InteractWidgetComponent->SetVisibility(false);

	bNetLoadOnClient = true;
	bReplicates = true;
}

void ANetherCrownInteractActor::BeginPlay()
{
	Super::BeginPlay();

	CacheInteractWidgetTexture();

	if (ensureAlways(IsValid(InteractDetectSphereComponent)))
	{
		InteractDetectSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandleOnDetectSphereOverlapBegin);
		InteractDetectSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::HandleOnDetectSphereOverlapEnd);
	}
}

void ANetherCrownInteractActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InteractTargetCharacter);
}

void ANetherCrownInteractActor::Interact()
{
	if (!IsValid(InteractTargetCharacter) || !HasAuthority())
	{
		return;
	}

	Multicast_SetInteractWidgetVisibility(InteractTargetCharacter, false);
}

void ANetherCrownInteractActor::HandleOnDetectSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent,
                                                                 AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                                 const FHitResult& SweepResult)
{
	InteractTargetCharacter = Cast<ANetherCrownCharacter>(OtherActor);
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
	InteractTargetCharacter = Cast<ANetherCrownCharacter>(OtherActor);
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

void ANetherCrownInteractActor::CacheInteractWidgetTexture()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (InteractWidgetTextureSoft.IsNull())
	{
		return;
	}

	CachedInteractWidgetTexture = InteractWidgetTextureSoft.LoadSynchronous();
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
