// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyPlayFoundCosmeticTask.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NetherCrown/Components/NetherCrownEnemyBTCosmeticComponent.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"

UNetherCrownEnemyPlayFoundCosmeticTask::UNetherCrownEnemyPlayFoundCosmeticTask()
{
	NodeName = TEXT("PlayFoundCosmetic");
	NeedFoundReactionBlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, NeedFoundReactionBlackboardKey));
}

EBTNodeResult::Type UNetherCrownEnemyPlayFoundCosmeticTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const ANetherCrownEnemy* ControlledEnemy{ GetControlledEnemy(OwnerComp, false) };
	if (!IsValid(ControlledEnemy))
	{
		return EBTNodeResult::Failed;
	}

	UNetherCrownEnemyBTCosmeticComponent* EnemyBTCosmeticComponent{ ControlledEnemy->GetEnemyBTCosmeticComponent() };
	if (!ensureAlways(IsValid(EnemyBTCosmeticComponent)))
	{
		return EBTNodeResult::Failed;
	}

	EnemyBTCosmeticComponent->PlayEnemyFoundCosmetic();

	UBlackboardComponent* BlackboardComponent{ GetBlackboardComponent(OwnerComp) };
	if (!IsValid(BlackboardComponent))
	{
		return EBTNodeResult::Failed;
	}

	BlackboardComponent->SetValueAsBool(NeedFoundReactionBlackboardKey.SelectedKeyName, false);

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
