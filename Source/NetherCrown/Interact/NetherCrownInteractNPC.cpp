// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownInteractNPC.h"

#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Character/NetherCrownPlayerController.h"
#include "NetherCrown/Components/NetherCrownInteractComponent.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/UI/NetherCrownUIManagerSubsystem.h"
#include "NetherCrown/Data/NetherCrownNPCData.h"
#include "NetherCrown/Widgets/NetherCrownNPCDialogueWidgetView.h"

ANetherCrownInteractNPC::ANetherCrownInteractNPC()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractDetectSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("InteractDetectSphereComponent"));
	InteractDetectSphereComponent->SetupAttachment(RootComponent);

	InteractWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidgetComponent"));
	InteractWidgetComponent->SetupAttachment(RootComponent);
	InteractWidgetComponent->SetVisibility(false);

	bNetLoadOnClient = true;
	bReplicates = true;
}

void ANetherCrownInteractNPC::BeginPlay()
{
	Super::BeginPlay();

	CacheNPCDataAsset();

	if (ensureAlways(IsValid(InteractDetectSphereComponent)))
	{
		InteractDetectSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandleOnDetectSphereOverlapBegin);
		InteractDetectSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::HandleOnDetectSphereOverlapEnd);
	}
}

void ANetherCrownInteractNPC::Interact()
{
	if (!HasAuthority())
	{
		return;
	}

	Multicast_ShowNPCDialogueWidget();
}

void ANetherCrownInteractNPC::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InteractTargetCharacterWeak);
	DOREPLIFETIME(ThisClass, CurrentQuestIndex);
}

void ANetherCrownInteractNPC::SetTargetInteractNPC(const ANetherCrownCharacter* InteractCharacter, bool bTargetValid)
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

	ANetherCrownInteractNPC* InteractNPC{ bTargetValid ? this : nullptr };
	InteractComponent->SetTargetInteractActor(InteractNPC);
}

void ANetherCrownInteractNPC::SetInteractWidgetVisibility(const ANetherCrownCharacter* InteractTarget,
	bool bVisible) const
{
	if (!InteractTarget->IsLocallyControlled())
	{
		return;
	}

	if (!ensureAlways(IsValid(InteractWidgetComponent)))
	{
		return;
	}

	InteractWidgetComponent->SetVisibility(bVisible);
}

void ANetherCrownInteractNPC::CacheNPCDataAsset()
{
	if (NPCDataAssetSoft.IsNull())
	{
		return;
	}

	CachedNPCDataAsset = NPCDataAssetSoft.LoadSynchronous();
}

void ANetherCrownInteractNPC::Multicast_ShowNPCDialogueWidget_Implementation()
{
	const ANetherCrownCharacter* InteractTargetCharacter{ InteractTargetCharacterWeak.Get() };
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

	if (!ensureAlways(IsValid(CachedNPCDataAsset)))
	{
		return;
	}

	const TArray<UNetherCrownQuestData*> QuestData{ CachedNPCDataAsset->GetQuestDataArray() };
	if (!(QuestData.IsValidIndex(CurrentQuestIndex)))
	{
		return;
	}

	NPCDialogueWidget->SetDialogueText(QuestData[CurrentQuestIndex]->GetQuestDialogues(), 0);
}

void ANetherCrownInteractNPC::HandleOnDetectSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent,
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
		SetTargetInteractNPC(InteractTargetCharacter, true);
	}
}

void ANetherCrownInteractNPC::HandleOnDetectSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent,
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
		SetTargetInteractNPC(InteractTargetCharacter, false);
	}
}
