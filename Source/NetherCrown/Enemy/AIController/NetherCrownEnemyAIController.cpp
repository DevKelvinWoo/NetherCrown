// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyAIController.h"

#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Net/UnrealNetwork.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ANetherCrownEnemyAIController::ANetherCrownEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = false;

	EnemyPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
}

void ANetherCrownEnemyAIController::HandleEnemyDead()
{
	UBehaviorTreeComponent* BTComponent{ Cast<UBehaviorTreeComponent>(BrainComponent) };
	if (!ensureAlways(IsValid(BTComponent)))
	{
		return;
	}

	if (ensureAlways(IsValid(BlackboardComponentCached)))
	{
		BlackboardComponentCached->ClearValue(EnemyAITuningData.TargetActorBlackboardKeyName);
		BlackboardComponentCached->ClearValue(EnemyAITuningData.NeedFoundReactionBlackboardKeyName);
		BlackboardComponentCached->ClearValue(EnemyAITuningData.ChaseTypeBlackboardKeyName);
	}

	if (ensureAlways(IsValid(EnemyPerceptionComponent)))
	{
		EnemyPerceptionComponent->SetActive(false);
	}

	BTComponent->StopTree(EBTStopMode::Safe);
}

void ANetherCrownEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	SetSightConfigAndPerceptionComponentValue();

	if (ensureAlways(IsValid(EnemyPerceptionComponent)))
	{
		EnemyPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::HandleTargetPerceptionUpdated);
	}
}

void ANetherCrownEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!HasAuthority())
	{
		return;
	}

	LoadEnemyAITuningData();

	if (!ensureAlways(IsValid(EnemyAITuningData.BehaviorTreeAsset)))
	{
		return;
	}

	UBlackboardComponent* BlackboardComponent{};
	UseBlackboard(EnemyAITuningData.BehaviorTreeAsset->BlackboardAsset, BlackboardComponent);
	if (!ensureAlways(IsValid(BlackboardComponent)))
	{
		return;
	}

	BlackboardComponentCached = BlackboardComponent;
	RunBehaviorTree(EnemyAITuningData.BehaviorTreeAsset);
}

void ANetherCrownEnemyAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!ensureAlways(IsValid(BlackboardComponentCached)))
	{
		return;
	}

	ANetherCrownCharacter* TargetCharacter{ Cast<ANetherCrownCharacter>(Actor) };
	if (!IsValid(TargetCharacter))
	{
		BlackboardComponentCached->ClearValue(EnemyAITuningData.TargetActorBlackboardKeyName);
		BlackboardComponentCached->ClearValue(EnemyAITuningData.NeedFoundReactionBlackboardKeyName);

		return;
	}

	ANetherCrownEnemy* OwnerEnemy{ Cast<ANetherCrownEnemy>(GetPawn()) };
	if (!ensureAlways(IsValid(OwnerEnemy)))
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		const UObject* TargetActor{ BlackboardComponentCached->GetValueAsObject(EnemyAITuningData.TargetActorBlackboardKeyName) };
		if (!IsValid(TargetActor))
		{
			BlackboardComponentCached->SetValueAsBool(EnemyAITuningData.NeedFoundReactionBlackboardKeyName, true);
		}

		BlackboardComponentCached->SetValueAsObject(EnemyAITuningData.TargetActorBlackboardKeyName, TargetCharacter);
		OwnerEnemy->SetCurrentTargetCharacter(TargetCharacter);
	}
	else
	{
		BlackboardComponentCached->ClearValue(EnemyAITuningData.ChaseTypeBlackboardKeyName);
		BlackboardComponentCached->ClearValue(EnemyAITuningData.TargetActorBlackboardKeyName);

		OwnerEnemy->SetCharacterMaxSpeed(false);
		OwnerEnemy->SetCurrentTargetCharacter(nullptr);
	}
}

void ANetherCrownEnemyAIController::LoadEnemyAITuningData()
{
	if (EnemyAITuningDataAssetSoft.IsNull() || !HasAuthority())
	{
		return;
	}

	const UNetherCrownEnemyAITuningDataAsset* EnemyAITuningDataAsset{ EnemyAITuningDataAssetSoft.LoadSynchronous() };
	if (!ensureAlways(IsValid(EnemyAITuningDataAsset)))
	{
		return;
	}

	EnemyAITuningData = EnemyAITuningDataAsset->GetEnemyAITuningData();
}

void ANetherCrownEnemyAIController::SetSightConfigAndPerceptionComponentValue()
{
	if (!HasAuthority())
	{
		return;
	}

	if (ensureAlways(IsValid(SightConfig)) && ensureAlways(IsValid(EnemyPerceptionComponent)))
	{
		SightConfig->SightRadius = EnemyAITuningData.PerceptionData.SightRadius;
		SightConfig->LoseSightRadius = EnemyAITuningData.PerceptionData.LoseSightRadius;
		SightConfig->PeripheralVisionAngleDegrees = EnemyAITuningData.PerceptionData.PeripheralVisionAngleDegrees;
		SightConfig->SetMaxAge(EnemyAITuningData.PerceptionData.MaxSightAge);
		SightConfig->AutoSuccessRangeFromLastSeenLocation = -1.0f;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

		EnemyPerceptionComponent->ConfigureSense(*SightConfig);
		EnemyPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
		SetPerceptionComponent(*EnemyPerceptionComponent);
	}
}
