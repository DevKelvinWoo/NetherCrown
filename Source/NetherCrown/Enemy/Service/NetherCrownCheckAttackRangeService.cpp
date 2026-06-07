// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownCheckAttackRangeService.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NetherCrown/Components/NetherCrownEnemyStatComponent.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"

UNetherCrownCheckAttackRangeService::UNetherCrownCheckAttackRangeService()
{
	NodeName = TEXT("Check Attack Range");

	TargetActorBlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, TargetActorBlackboardKey), AActor::StaticClass());
	AttackRangeBlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, AttackRangeBlackboardKey));

	Interval = 0.1f;
	RandomDeviation = 0.0f;
}

void UNetherCrownCheckAttackRangeService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, const float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const ANetherCrownEnemy* ControlledEnemy{ GetControlledEnemy(OwnerComp) };
	if (!IsValid(ControlledEnemy))
	{
		return;
	}

	UBlackboardComponent* BlackboardComponent{ GetBlackboardComponent(OwnerComp) };
	if (!IsValid(BlackboardComponent))
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
