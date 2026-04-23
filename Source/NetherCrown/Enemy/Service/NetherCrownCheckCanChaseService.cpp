// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownCheckCanChaseService.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Enemy/AIController/NetherCrownEnemyAIController.h"
#include "NetherCrown/Enemy/Components/NetherCrownEnemyActionControlComponent.h"

UNetherCrownCheckCanChaseService::UNetherCrownCheckCanChaseService()
{
	NodeName = TEXT("CheckCanChase");

	CanChaseBlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, CanChaseBlackboardKey));

	Interval = 0.1f;
	RandomDeviation = 0.0f;
}

void UNetherCrownCheckCanChaseService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const ANetherCrownEnemyAIController* EnemyAIController{ Cast<ANetherCrownEnemyAIController>(OwnerComp.GetAIOwner()) };
	if (!ensureAlways(IsValid(EnemyAIController)))
	{
		return;
	}

	const ANetherCrownEnemy* OwnerEnemy{ Cast<ANetherCrownEnemy>(EnemyAIController->GetPawn()) };
	if (!ensureAlways(IsValid(OwnerEnemy)))
	{
		return;
	}

	const UNetherCrownEnemyActionControlComponent* EnemyActionControlComponent{ OwnerEnemy->GetEnemyActionControlComponent() };
	if (!ensureAlways(IsValid(EnemyActionControlComponent)))
	{
		return;
	}

	UBlackboardComponent* BlackboardComponent{ OwnerComp.GetBlackboardComponent() };
	if (!ensureAlways(IsValid(BlackboardComponent)))
	{
		return;
	}

	BlackboardComponent->SetValueAsBool(CanChaseBlackboardKey.SelectedKeyName, EnemyActionControlComponent->CanChase());
}
