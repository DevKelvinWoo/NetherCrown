// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownFindPatrolLocationTask.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UNetherCrownFindPatrolLocationTask::UNetherCrownFindPatrolLocationTask()
{
	NodeName = TEXT("Find Patrol Location");
	PatrolLocationBlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, PatrolLocationBlackboardKey));
}

EBTNodeResult::Type UNetherCrownFindPatrolLocationTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* OwnerAIController{ OwnerComp.GetAIOwner() };
	if (!ensureAlways(IsValid(OwnerAIController)))
	{
		return EBTNodeResult::Failed;
	}

	APawn* ControlledPawn{ OwnerAIController->GetPawn() };
	if (!ensureAlways(IsValid(ControlledPawn)))
	{
		return EBTNodeResult::Failed;
	}

	UNavigationSystemV1* NavigationSystem{ FNavigationSystem::GetCurrent<UNavigationSystemV1>(ControlledPawn->GetWorld()) };
	if (!ensureAlways(IsValid(NavigationSystem)))
	{
		return EBTNodeResult::Failed;
	}

	FNavLocation RandomPatrolLocation{};
	const bool bFoundPatrolLocation{ NavigationSystem->GetRandomReachablePointInRadius(ControlledPawn->GetActorLocation(), PatrolRadius, RandomPatrolLocation) };
	if (!bFoundPatrolLocation)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComponent{ OwnerComp.GetBlackboardComponent() };
	if (!ensureAlways(IsValid(BlackboardComponent)))
	{
		return EBTNodeResult::Failed;
	}

	BlackboardComponent->SetValueAsVector(PatrolLocationBlackboardKey.SelectedKeyName, RandomPatrolLocation.Location);

	return EBTNodeResult::Succeeded;
}
