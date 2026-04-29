// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownBossRangedMoveLocationTask.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Enemy/AIController/NetherCrownEnemyAIController.h"

UNetherCrownBossRangedMoveLocationTask::UNetherCrownBossRangedMoveLocationTask()
{
	NodeName = TEXT("Boss Ranged Move Location");

	TargetActorBlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, TargetActorBlackboardKey), AActor::StaticClass());
	MoveLocationBlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, MoveLocationBlackboardKey));
}

EBTNodeResult::Type UNetherCrownBossRangedMoveLocationTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	const ANetherCrownEnemyAIController* EnemyAIController{ Cast<ANetherCrownEnemyAIController>(OwnerComp.GetAIOwner()) };
	if (!ensureAlways(IsValid(EnemyAIController)))
	{
		return EBTNodeResult::Failed;
	}

	const ANetherCrownEnemy* OwnerEnemy{ Cast<ANetherCrownEnemy>(EnemyAIController->GetPawn()) };
	if (!ensureAlways(IsValid(OwnerEnemy)) || !OwnerEnemy->HasAuthority())
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComponent{ OwnerComp.GetBlackboardComponent() };
	if (!ensureAlways(IsValid(BlackboardComponent)))
	{
		return EBTNodeResult::Failed;
	}

	const AActor* TargetActor{ Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetActorBlackboardKey.SelectedKeyName)) };
	if (!ensureAlways(IsValid(TargetActor)))
	{
		return EBTNodeResult::Failed;
	}

	const UNavigationSystemV1* NavigationSystem{ FNavigationSystem::GetCurrent<UNavigationSystemV1>(OwnerEnemy->GetWorld()) };
	if (!ensureAlways(IsValid(NavigationSystem)))
	{
		return EBTNodeResult::Failed;
	}

	const FVector OwnerLocation{ OwnerEnemy->GetActorLocation() };
	const FVector TargetLocation{ TargetActor->GetActorLocation() };
	const ENetherCrownBossRangedMoveType ResolvedMoveType{ ResolveMoveType(OwnerLocation, TargetLocation) };
	const FVector DesiredDirection{ GetDesiredDirectionFromTarget(OwnerLocation, TargetLocation, ResolvedMoveType) };
	const FVector DesiredLocation{ TargetLocation + DesiredDirection * PreferredAttackDistance };

	FNavLocation MoveLocation{};
	const bool bFoundLocation{ NavigationSystem->GetRandomReachablePointInRadius(DesiredLocation, LocationSearchRadius, MoveLocation) };
	if (!bFoundLocation)
	{
		return EBTNodeResult::Failed;
	}

	BlackboardComponent->SetValueAsVector(MoveLocationBlackboardKey.SelectedKeyName, MoveLocation.Location);

	return EBTNodeResult::Succeeded;
}

ENetherCrownBossRangedMoveType UNetherCrownBossRangedMoveLocationTask::ResolveMoveType(const FVector& OwnerLocation, const FVector& TargetLocation) const
{
	if (MoveType != ENetherCrownBossRangedMoveType::Auto)
	{
		return MoveType;
	}

	const double DistanceSquared{ FVector::DistSquared2D(OwnerLocation, TargetLocation) };
	if (DistanceSquared <= FMath::Square(MinAttackDistance))
	{
		return ENetherCrownBossRangedMoveType::Escape;
	}

	if (DistanceSquared >= FMath::Square(MaxAttackDistance))
	{
		return ENetherCrownBossRangedMoveType::Approach;
	}

	return ENetherCrownBossRangedMoveType::Orbit;
}

FVector UNetherCrownBossRangedMoveLocationTask::GetDesiredDirectionFromTarget(const FVector& OwnerLocation, const FVector& TargetLocation, const ENetherCrownBossRangedMoveType ResolvedMoveType) const
{
	const FVector TargetToOwnerDirection{ (OwnerLocation - TargetLocation).GetSafeNormal2D() };
	if (TargetToOwnerDirection.IsNearlyZero())
	{
		return FVector::ForwardVector;
	}

	if (ResolvedMoveType == ENetherCrownBossRangedMoveType::Approach)
	{
		return TargetToOwnerDirection;
	}

	if (ResolvedMoveType == ENetherCrownBossRangedMoveType::Escape)
	{
		return TargetToOwnerDirection;
	}

	const float DirectionSign{ FMath::RandBool() ? 1.0f : -1.0f };
	return TargetToOwnerDirection.RotateAngleAxis(OrbitAngleDegrees * DirectionSign, FVector::UpVector).GetSafeNormal2D();
}
