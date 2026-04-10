// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemySelectChaseTypeTask.h"

#include "BehaviorTree/BlackboardComponent.h"

UNetherCrownEnemySelectChaseTypeTask::UNetherCrownEnemySelectChaseTypeTask()
{
	NodeName = TEXT("SelectChaseType");
	ChaseTypeBlackboardKey.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, ChaseTypeBlackboardKey), StaticEnum<ENetherCrownEnemyChaseType>());
}

EBTNodeResult::Type UNetherCrownEnemySelectChaseTypeTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComponent{ OwnerComp.GetBlackboardComponent() };
	if (!ensureAlways(IsValid(BlackboardComponent)))
	{
		return EBTNodeResult::Failed;
	}

	const ENetherCrownEnemyChaseType ChaseType{ BlackboardComponent->GetValueAsEnum(ChaseTypeBlackboardKey.SelectedKeyName) };
	if (ChaseType != ENetherCrownEnemyChaseType::None)
	{
		return EBTNodeResult::Succeeded;
	}

	const ENetherCrownEnemyChaseType SelectedChaseType{ FMath::FRand() <= 0.25f ? ENetherCrownEnemyChaseType::DashChase : ENetherCrownEnemyChaseType::RunChase };
	BlackboardComponent->SetValueAsEnum(ChaseTypeBlackboardKey.SelectedKeyName, static_cast<uint8>(SelectedChaseType));

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
