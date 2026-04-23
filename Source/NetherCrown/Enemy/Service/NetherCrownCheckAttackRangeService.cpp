// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownCheckAttackRangeService.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NetherCrown/Components/NetherCrownEnemyStatComponent.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Enemy/AIController/NetherCrownEnemyAIController.h"

UNetherCrownCheckAttackRangeService::UNetherCrownCheckAttackRangeService()
{
	NodeName = TEXT("Check Attack Range");

	TargetActorBlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, TargetActorBlackboardKey), AActor::StaticClass());
	AttackRangeBlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, AttackRangeBlackboardKey));

	Interval = 0.1f;
	RandomDeviation = 0.0f;
}

void UNetherCrownCheckAttackRangeService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const ANetherCrownEnemyAIController* EnemyAIController{ Cast<ANetherCrownEnemyAIController>(OwnerComp.GetAIOwner()) };
	if (!ensureAlways(IsValid(EnemyAIController)))
	{
		return;
	}

	const ANetherCrownEnemy* ControlledEnemy{ Cast<ANetherCrownEnemy>(EnemyAIController->GetPawn()) };
	if (!ensureAlways(IsValid(ControlledEnemy)) || !ControlledEnemy->HasAuthority())
	{
		return;
	}

	UBlackboardComponent* BlackboardComponent{ OwnerComp.GetBlackboardComponent() };
	if (!ensureAlways(IsValid(BlackboardComponent)))
	{
		return;
	}

	AActor* TargetActor{ Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetActorBlackboardKey.SelectedKeyName)) };
	if (!IsValid(TargetActor))
	{
		BlackboardComponent->SetValueAsBool(AttackRangeBlackboardKey.SelectedKeyName, false);
		return;
	}

	const double DistanceSquared{ FVector::DistSquared(ControlledEnemy->GetActorLocation(), TargetActor->GetActorLocation()) };
	const UNetherCrownEnemyStatComponent* EnemyStatComponent{ ControlledEnemy->GetEnemyStatComponent() };
	if (!ensureAlways(IsValid(EnemyStatComponent)))
	{
		return;
	}

	const int32 EnemyAttackRange{ EnemyStatComponent->GetEnemyStatData().AttackRange };
	const bool bInAttackRange{ DistanceSquared <= FMath::Square(EnemyAttackRange) };
	BlackboardComponent->SetValueAsBool(AttackRangeBlackboardKey.SelectedKeyName, bInAttackRange);
}
