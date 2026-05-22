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

	const ANetherCrownCharacter* InteractTargetCharacter{ InteractTargetCharacterWeak.Get() };
	Multicast_SetInteractWidgetVisibility(InteractTargetCharacter, false);
	Multicast_ShowNPCDialogueWidget(GetCurrentQuestIndexForPlayer(InteractTargetCharacter), GetTargetPlayerQuestState());
}

void ANetherCrownInteractNPC::FinishInteract(ANetherCrownCharacter* InteractCharacter)
{
	if (!ensureAlways(IsValid(InteractCharacter)) || !InteractCharacter->HasAuthority())
	{
		return;
	}

	Multicast_SetInteractWidgetVisibility(InteractCharacter, true);

	if (!ensureAlways(IsValid(CachedNPCDataAsset)))
	{
		return;
	}

	const int32 CurrentQuestIndex{ GetCurrentQuestIndexForPlayer(InteractCharacter) };
	if (!CachedNPCDataAsset->GetQuestDataArray().IsValidIndex(CurrentQuestIndex))
	{
		return;
	}

	const UNetherCrownQuestData* CurrentQuestData{ CachedNPCDataAsset->GetQuestDataArray()[CurrentQuestIndex] };
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
	const ENetherCrownQuestState QuestState{ QuestComponent->GetQuestState(QuestTag) };
	if (QuestState == ENetherCrownQuestState::Done)
	{
		QuestComponent->RequestGrantQuestReward(QuestTag);
	}
	else if (QuestState == ENetherCrownQuestState::None)
	{
		QuestComponent->RequestAcceptQuestState(QuestTag, ENetherCrownQuestState::InProgress);
	}
}

void ANetherCrownInteractNPC::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InteractTargetCharacterWeak);
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

int32 ANetherCrownInteractNPC::GetCurrentQuestIndexForPlayer(const ANetherCrownCharacter* InteractCharacter) const
{
	if (!ensureAlways(IsValid(InteractCharacter)))
	{
		return INDEX_NONE;
	}

	if (!ensureAlways(IsValid(CachedNPCDataAsset)))
	{
		return INDEX_NONE;
	}

	const UNetherCrownQuestComponent* QuestComponent{ InteractCharacter->GetQuestComponent() };
	if (!ensureAlways(IsValid(QuestComponent)))
	{
		return INDEX_NONE;
	}

	const TArray<TObjectPtr<UNetherCrownQuestData>>& QuestDataArray{ CachedNPCDataAsset->GetQuestDataArray() };
	for (int32 QuestIndex{ 0 }; QuestIndex < QuestDataArray.Num(); ++QuestIndex)
	{
		const UNetherCrownQuestData* QuestData{ QuestDataArray[QuestIndex] };
		if (!IsValid(QuestData))
		{
			continue;
		}

		if (QuestComponent->GetQuestState(QuestData->GetQuestTag()) != ENetherCrownQuestState::Rewarded)
		{
			return QuestIndex;
		}
	}

	return INDEX_NONE;
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

	const int32 CurrentQuestIndex{ GetCurrentQuestIndexForPlayer(InteractTargetCharacter) };
	if (!CachedNPCDataAsset->GetQuestDataArray().IsValidIndex(CurrentQuestIndex))
	{
		return ENetherCrownQuestState::None;
	}

	const UNetherCrownQuestComponent* QuestComponent{ InteractTargetCharacter->GetQuestComponent() };
	if (!ensureAlways(IsValid(QuestComponent)))
	{
		return ENetherCrownQuestState::None;
	}

	const UNetherCrownQuestData* CurrentQuestData{ CachedNPCDataAsset->GetQuestDataArray()[CurrentQuestIndex] };
	const ENetherCrownQuestState QuestState{ QuestComponent->GetQuestState(CurrentQuestData->GetQuestTag()) };

	return QuestState;
}

TArray<FText> ANetherCrownInteractNPC::GetQuestDialogueText(const int32 QuestIndex, const ENetherCrownQuestState InQuestState) const
{
	const TArray<UNetherCrownQuestData*> QuestData{ CachedNPCDataAsset->GetQuestDataArray() };
	if (!(QuestData.IsValidIndex(QuestIndex)))
	{
		return {};
	}

	TArray<FText> DialogueTexts{};
	switch (InQuestState)
	{
	case ENetherCrownQuestState::None:
		DialogueTexts = QuestData[QuestIndex]->GetQuestDialogues();
		break;
	case ENetherCrownQuestState::InProgress:
		DialogueTexts = QuestData[QuestIndex]->GetQuestInprogressDialogues();
		break;
	case ENetherCrownQuestState::Done:
		DialogueTexts = QuestData[QuestIndex]->GetQuestEndDialogues();
		break;
	default:
		DialogueTexts = {};
		break;
	}

	return DialogueTexts;
}

void ANetherCrownInteractNPC::Multicast_ShowNPCDialogueWidget_Implementation(const int32 QuestIndex, const ENetherCrownQuestState QuestState)
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
	TArray<FText> DialogueText{ GetQuestDialogueText(QuestIndex, QuestState) };
	if (!(QuestData.IsValidIndex(QuestIndex)))
	{
		DialogueText = { CachedNPCDataAsset->GetNonQuestNPCDialogues() };
	}

	NPCDialogueWidget->InitViewModel(InteractTargetCharacter);
	NPCDialogueWidget->SetDialogueText(DialogueText, 0);
}

void ANetherCrownInteractNPC::Multicast_SetInteractWidgetVisibility_Implementation(
	const ANetherCrownCharacter* InteractTarget, bool bVisible)
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (!ensureAlways(IsValid(InteractTarget)))
	{
		return;
	}

	SetInteractWidgetVisibility(InteractTarget, bVisible);
}

void ANetherCrownInteractNPC::HandleOnDetectSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent,
                                                               AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                               const FHitResult& SweepResult)
{
	ANetherCrownCharacter* InteractTargetCharacter{ Cast<ANetherCrownCharacter>(OtherActor) };
	if (!IsValid(InteractTargetCharacter))
	{
		return;
	}

	InteractTargetCharacterWeak = MakeWeakObjectPtr(InteractTargetCharacter);

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
	ANetherCrownCharacter* InteractTargetCharacter{ Cast<ANetherCrownCharacter>(OtherActor) };
	if (!IsValid(InteractTargetCharacter))
	{
		return;
	}

	InteractTargetCharacterWeak = MakeWeakObjectPtr(InteractTargetCharacter);

	if (InteractTargetCharacter->IsLocallyControlled())
	{
		SetInteractWidgetVisibility(InteractTargetCharacter, false);
	}

	if (HasAuthority())
	{
		SetTargetInteractNPC(InteractTargetCharacter, false);
	}
}
