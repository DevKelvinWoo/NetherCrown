// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownSequenceInteractActor.h"

#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"
#include "Runtime/LevelSequence/Public/LevelSequence.h"

ANetherCrownSequenceInteractActor::ANetherCrownSequenceInteractActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	InteractBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractBoxComponent"));
	InteractBoxComponent->SetupAttachment(RootComponent);

	bNetLoadOnClient = true;
	bReplicates = true;
}

void ANetherCrownSequenceInteractActor::BeginPlay()
{
	Super::BeginPlay();

	if (ensureAlways(IsValid(InteractBoxComponent)))
	{
		InteractBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandleOnInteractBoxBeginOverlap);
	}
}

void ANetherCrownSequenceInteractActor::GetLifetimeReplicatedProps(
	TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InteractCharacterWeak);
}

void ANetherCrownSequenceInteractActor::ToggleInteractBoxCollision(const bool bOn)
{
	if (!HasAuthority())
	{
		return;
	}

	Multicast_SetInteractBoxCollision(bOn);
}

void ANetherCrownSequenceInteractActor::Multicast_SetInteractBoxCollision_Implementation(const bool bOn)
{
	if (!ensureAlways(IsValid(InteractBoxComponent)))
	{
		return;
	}

	InteractBoxComponent->SetGenerateOverlapEvents(bOn);
	InteractBoxComponent->SetCollisionEnabled(bOn ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void ANetherCrownSequenceInteractActor::HandleOnInteractBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                                                        AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                                        const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	ANetherCrownCharacter* InteractCharacter{ Cast<ANetherCrownCharacter>(OtherActor) };
	if (!IsValid(InteractCharacter) || !LevelSequenceTag.IsValid())
	{
		return;
	}

	InteractCharacterWeak = MakeWeakObjectPtr(InteractCharacter);
	Multicast_PlaySequence();
}

void ANetherCrownSequenceInteractActor::Multicast_PlaySequence_Implementation()
{
	ULevelSequence* TargetLevelSequence{ FNetherCrownUtilManager::GetLevelSequenceByGameplayTag(LevelSequenceTag) };
	if (!ensureAlways(IsValid(TargetLevelSequence)))
	{
		return;
	}

	FMovieSceneSequencePlaybackSettings PlaybackSettings{};
	PlaybackSettings.bPauseAtEnd = true;

	ALevelSequenceActor* SequenceActor{};
	ULevelSequencePlayer* SequencePlayer{
		ULevelSequencePlayer::CreateLevelSequencePlayer(
			GetWorld(),
			TargetLevelSequence,
			PlaybackSettings,
			SequenceActor
		)
	};

	if (!ensureAlways(IsValid(SequencePlayer)))
	{
		return;
	}

	SequencePlayer->OnFinished.AddDynamic(this, &ThisClass::HandleOnSequenceFinished);
	SequencePlayer->Play();
}
