// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownUpdateRangedBossMovementService.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NetherCrown/Data/NetherCrownEnemyAITuningData.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Enemy/AIController/NetherCrownEnemyAIController.h"
#include "NetherCrown/Enemy/Components/NetherCrownEnemyActionControlComponent.h"

UNetherCrownUpdateRangedBossMovementService::UNetherCrownUpdateRangedBossMovementService()
{
	NodeName = TEXT("Update Ranged Boss Movement");

	TargetActorBlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, TargetActorBlackboardKey), AActor::StaticClass());
	TargetDistanceBlackboardKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, TargetDistanceBlackboardKey));
	NeedEscapeBlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, NeedEscapeBlackboardKey));
	NeedApproachBlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, NeedApproachBlackboardKey));
	NeedOrbitBlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, NeedOrbitBlackboardKey));

	Interval = 0.1f;
	RandomDeviation = 0.0f;
}

void UNetherCrownUpdateRangedBossMovementService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const ANetherCrownEnemyAIController* EnemyAIController{ Cast<ANetherCrownEnemyAIController>(OwnerComp.GetAIOwner()) };
	if (!ensureAlways(IsValid(EnemyAIController)))
	{
		return;
	}

	const ANetherCrownEnemy* OwnerEnemy{ Cast<ANetherCrownEnemy>(EnemyAIController->GetPawn()) };
	if (!ensureAlways(IsValid(OwnerEnemy)) || !OwnerEnemy->HasAuthority())
	{
		return;
	}

	UBlackboardComponent* BlackboardComponent{ OwnerComp.GetBlackboardComponent() };
	if (!ensureAlways(IsValid(BlackboardComponent)))
	{
		return;
	}

	const UNetherCrownEnemyActionControlComponent* EnemyActionControlComponent{ OwnerEnemy->GetEnemyActionControlComponent() };
	if (!ensureAlways(IsValid(EnemyActionControlComponent)) || !EnemyActionControlComponent->CanChase())
	{
		ClearMovementState(BlackboardComponent);
		return;
	}

	const AActor* TargetActor{ Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetActorBlackboardKey.SelectedKeyName)) };
	if (!IsValid(TargetActor))
	{
		ClearMovementState(BlackboardComponent);
		return;
	}

	const double TargetDistance{ FVector::Dist2D(OwnerEnemy->GetActorLocation(), TargetActor->GetActorLocation()) };
	const FNetherCrownBossRangedCombatData& BossRangedCombatData{ EnemyAIController->GetBossRangedCombatData() };
	const bool bNeedEscape{ TargetDistance <= BossRangedCombatData.MinAttackDistance };
	const bool bNeedApproach{ TargetDistance >= BossRangedCombatData.MaxAttackDistance };
	const bool bNeedOrbit{ !bNeedEscape && !bNeedApproach };

	BlackboardComponent->SetValueAsFloat(TargetDistanceBlackboardKey.SelectedKeyName, TargetDistance);
	BlackboardComponent->SetValueAsBool(NeedEscapeBlackboardKey.SelectedKeyName, bNeedEscape);
	BlackboardComponent->SetValueAsBool(NeedApproachBlackboardKey.SelectedKeyName, bNeedApproach);
	BlackboardComponent->SetValueAsBool(NeedOrbitBlackboardKey.SelectedKeyName, bNeedOrbit);
}

void UNetherCrownUpdateRangedBossMovementService::ClearMovementState(UBlackboardComponent* BlackboardComponent) const
{
	if (!ensureAlways(IsValid(BlackboardComponent)))
	{
		return;
	}

	BlackboardComponent->SetValueAsFloat(TargetDistanceBlackboardKey.SelectedKeyName, 0.0f);
	BlackboardComponent->SetValueAsBool(NeedEscapeBlackboardKey.SelectedKeyName, false);
	BlackboardComponent->SetValueAsBool(NeedApproachBlackboardKey.SelectedKeyName, false);
	BlackboardComponent->SetValueAsBool(NeedOrbitBlackboardKey.SelectedKeyName, false);
}
