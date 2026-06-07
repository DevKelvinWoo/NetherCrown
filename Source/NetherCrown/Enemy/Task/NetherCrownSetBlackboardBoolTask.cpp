// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownSetBlackboardBoolTask.h"

#include "BehaviorTree/BlackboardComponent.h"

UNetherCrownSetBlackboardBoolTask::UNetherCrownSetBlackboardBoolTask()
{
	NodeName = TEXT("Set Blackboard Bool");

	BlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, BlackboardKey));
}

EBTNodeResult::Type UNetherCrownSetBlackboardBoolTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComponent{ GetBlackboardComponent(OwnerComp) };
	if (!IsValid(BlackboardComponent))
	{
		return EBTNodeResult::Failed;
	}

	BlackboardComponent->SetValueAsBool(BlackboardKey.SelectedKeyName, bValue);

	return EBTNodeResult::Succeeded;
}
