// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownInteractNPC.h"

#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Character/NetherCrownPlayerController.h"
#include "NetherCrown/Components/NetherCrownInteractComponent.h"
#include "NetherCrown/Components/NetherCrownQuestComponent.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/UI/NetherCrownUIManagerSubsystem.h"
#include "NetherCrown/Data/NetherCrownNPCData.h"
#include "NetherCrown/Data/NetherCrownQuestData.h"
#include "NetherCrown/Widgets/NetherCrownNPCDialogueWidgetView.h"

ANetherCrownInteractNPC::ANetherCrownInteractNPC()
{
	PrimaryActorTick.bCanEverTick = false;

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

FVector ANetherCrownInteractNPC::GetInteractCameraPos() const
{
	if (!ensureAlways(IsValid(InteractCameraPosSphereComponent)))
	{
		return FVector::ZeroVector;
	}

	return InteractCameraPosSphereComponent->GetComponentLocation();
}

FRotator ANetherCrownInteractNPC::GetInteractCameraRot() const
{
	if (!ensureAlways(IsValid(InteractCameraPosSphereComponent)))
	{
		return FRotator::ZeroRotator;
	}

	return InteractCameraPosSphereComponent->GetComponentRotation();
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

	Multicast_ShowNPCDialogueWidget(GetTargetPlayerQuestState());
}

void ANetherCrownInteractNPC::FinishInteract(ANetherCrownCharacter* InteractCharacter)
{
	if (!ensureAlways(IsValid(InteractCharacter)) || !InteractCharacter->HasAuthority())
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

	const UNetherCrownQuestData* CurrentQuestData{ QuestData[CurrentQuestIndex] };
	if (!ensureAlways(IsValid(CurrentQuestData)))
	{
		return;
	}

	UNetherCrownQuestComponent* QuestComponent{ InteractCharacter->GetQuestComponent() };
	if (!ensureAlways(IsValid(QuestComponent)))
	{
		return;
	}

	const FGameplayTag& QuestTag{ CurrentQuestData->GetQuestTag() };
	if (QuestComponent->GetQuestState(QuestTag) == ENetherCrownQuestState::Done)
	{
		++CurrentQuestIndex;
		QuestComponent->RequestGrantQuestReward(QuestTag);
	}
	else
	{
		QuestComponent->RequestAcceptQuestState(QuestTag, ENetherCrownQuestState::InProgress);
	}
}

void ANetherCrownInteractNPC::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InteractTargetCharacterWeak);
	DOREPLIFETIME(ThisClass, CurrentQuestIndex);
}

void ANetherCrownInteractNPC::SetTargetInteractNPC(const ANetherCrownCharacter* InteractCharacter, bool bTargetValid)
{
	if (!HasAuthority() || !IsValid(InteractCharacter))
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

const ENetherCrownQuestState ANetherCrownInteractNPC::GetTargetPlayerQuestState() const
{
	ANetherCrownCharacter* InteractTargetCharacter{ InteractTargetCharacterWeak.Get() };
	if (!ensureAlways(IsValid(InteractTargetCharacter)) || !InteractTargetCharacter->HasAuthority())
	{
		return ENetherCrownQuestState::None;
	}

	if (!ensureAlways(IsValid(CachedNPCDataAsset)))
	{
		return ENetherCrownQuestState::None;
	}

	const TArray<UNetherCrownQuestData*> QuestData{ CachedNPCDataAsset->GetQuestDataArray() };
	if (!(QuestData.IsValidIndex(CurrentQuestIndex)))
	{
		return ENetherCrownQuestState::None;
	}

	const UNetherCrownQuestComponent* QuestComponent{ InteractTargetCharacter->GetQuestComponent() };
	if (!ensureAlways(IsValid(QuestComponent)))
	{
		return ENetherCrownQuestState::None;
	}

	const UNetherCrownQuestData* CurrentQuestData{ QuestData[CurrentQuestIndex] };
	const ENetherCrownQuestState QuestState{ QuestComponent->GetQuestState(CurrentQuestData->GetQuestTag()) };

	return QuestState;
}

TArray<FText> ANetherCrownInteractNPC::GetQuestDialogueText(const ENetherCrownQuestState InQuestState) const
{
	const TArray<UNetherCrownQuestData*> QuestData{ CachedNPCDataAsset->GetQuestDataArray() };
	if (!(QuestData.IsValidIndex(CurrentQuestIndex)))
	{
		return {};
	}

	TArray<FText> DialogueTexts{};
	switch (InQuestState)
	{
	case ENetherCrownQuestState::None:
		DialogueTexts = QuestData[CurrentQuestIndex]->GetQuestDialogues();
		break;
	case ENetherCrownQuestState::InProgress:
		DialogueTexts = QuestData[CurrentQuestIndex]->GetQuestInprogressDialogues();
		break;
	case ENetherCrownQuestState::Done:
		DialogueTexts = QuestData[CurrentQuestIndex]->GetQuestEndDialogues();
		break;
	default:
		DialogueTexts = {};
		break;
	}

	return DialogueTexts;
}

void ANetherCrownInteractNPC::Multicast_ShowNPCDialogueWidget_Implementation(const ENetherCrownQuestState QuestState)
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

	if (!ensureAlways(IsValid(CachedNPCDataAsset)))
	{
		return;
	}

	const TArray<UNetherCrownQuestData*> QuestData{ CachedNPCDataAsset->GetQuestDataArray() };
	if (!(QuestData.IsValidIndex(CurrentQuestIndex)))
	{
		return;
	}

	NPCDialogueWidget->InitViewModel(InteractTargetCharacter);
	NPCDialogueWidget->SetDialogueText(GetQuestDialogueText(QuestState), 0);
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
