// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyAIController.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ANetherCrownEnemyAIController::ANetherCrownEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = false;

	EnemyPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SetSightConfigAndPerceptionComponentValue();
}

void ANetherCrownEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	if (ensureAlways(IsValid(EnemyPerceptionComponent)))
	{
		EnemyPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::HandleTargetPerceptionUpdated);
	}
}

void ANetherCrownEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!ensureAlways(IsValid(BehaviorTreeAsset)))
	{
		return;
	}

	UBlackboardComponent* BlackboardComponent{};
	UseBlackboard(BehaviorTreeAsset->BlackboardAsset, BlackboardComponent);
	if (!ensureAlways(IsValid(BlackboardComponent)))
	{
		return;
	}

	BlackboardComponentCached = BlackboardComponent;
	RunBehaviorTree(BehaviorTreeAsset);
}

void ANetherCrownEnemyAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!HasAuthority())
	{
		return;
	}

	ANetherCrownCharacter* TargetCharacter{ Cast<ANetherCrownCharacter>(Actor) };
	if (!IsValid(TargetCharacter))
	{
		return;
	}

	if (!ensureAlways(IsValid(BlackboardComponentCached)))
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		BlackboardComponentCached->SetValueAsObject(TargetActorBlackboardKey, TargetCharacter);
		return;
	}

	const UObject* CurrentTargetObject{ BlackboardComponentCached->GetValueAsObject(TargetActorBlackboardKey) };
	if (CurrentTargetObject == TargetCharacter)
	{
		BlackboardComponentCached->ClearValue(TargetActorBlackboardKey);
		StopMovement();
	}
}

void ANetherCrownEnemyAIController::SetSightConfigAndPerceptionComponentValue()
{
	if (ensureAlways(IsValid(SightConfig)) && ensureAlways(IsValid(EnemyPerceptionComponent)))
	{
		SightConfig->SightRadius = SightRadius;
		SightConfig->LoseSightRadius = LoseSightRadius;
		SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngleDegrees;
		SightConfig->SetMaxAge(MaxSightAge);
		SightConfig->AutoSuccessRangeFromLastSeenLocation = -1.0f;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

		EnemyPerceptionComponent->ConfigureSense(*SightConfig);
		EnemyPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
		SetPerceptionComponent(*EnemyPerceptionComponent);
	}
}
