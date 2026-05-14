// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownSequenceInteractActor.h"

#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "Components/BoxComponent.h"
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

void ANetherCrownSequenceInteractActor::HandleOnInteractBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!OtherActor->IsA<ANetherCrownCharacter>() || !LevelSequenceTag.IsValid())
	{
		return;
	}

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
