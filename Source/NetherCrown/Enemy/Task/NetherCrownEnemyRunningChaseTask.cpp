// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyRunningChaseTask.h"

#include "NetherCrownEnemySelectChaseTypeTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"

UNetherCrownEnemyRunningChaseTask::UNetherCrownEnemyRunningChaseTask()
{
	NodeName = TEXT("RunningChase");
	bCreateNodeInstance = true;

	ChaseTypeBlackboardKey.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, ChaseTypeBlackboardKey), StaticEnum<ENetherCrownEnemyChaseType>());
	NeedRunChaseBlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, NeedRunChaseBlackboardKey));
}

EBTNodeResult::Type UNetherCrownEnemyRunningChaseTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComponent{ GetBlackboardComponent(OwnerComp) };
	if (!IsValid(BlackboardComponent))
	{
		return EBTNodeResult::Failed;
	}

	ANetherCrownEnemy* OwnerEnemy{ GetControlledEnemy(OwnerComp) };
	if (!IsValid(OwnerEnemy))
	{
		return EBTNodeResult::Failed;
	}

	const ENetherCrownEnemyChaseType ChaseType{ BlackboardComponent->GetValueAsEnum(ChaseTypeBlackboardKey.SelectedKeyName) };
	if (ChaseType != ENetherCrownEnemyChaseType::RunChase)
	{
		return EBTNodeResult::Succeeded;
	}

	OwnerEnemy->SetCharacterMaxSpeed(true);

	BlackboardComponent->SetValueAsEnum(ChaseTypeBlackboardKey.SelectedKeyName, static_cast<uint8>(ENetherCrownEnemyChaseType::RunChase));
	BlackboardComponent->SetValueAsBool(NeedRunChaseBlackboardKey.SelectedKeyName, false);

	return EBTNodeResult::Succeeded;
}
