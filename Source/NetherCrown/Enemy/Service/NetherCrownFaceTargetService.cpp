// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownFaceTargetService.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Enemy/AIController/NetherCrownEnemyAIController.h"
#include "NetherCrown/Enemy/Components/NetherCrownEnemyActionControlComponent.h"

UNetherCrownFaceTargetService::UNetherCrownFaceTargetService()
{
	NodeName = TEXT("Face Target");

	TargetActorBlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, TargetActorBlackboardKey), AActor::StaticClass());
	BlockFaceTargetBlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, BlockFaceTargetBlackboardKey));

	Interval = 0.05f;
	RandomDeviation = 0.0f;
}

void UNetherCrownFaceTargetService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, const float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ANetherCrownEnemyAIController* EnemyAIController{ Cast<ANetherCrownEnemyAIController>(OwnerComp.GetAIOwner()) };
	if (!ensureAlways(IsValid(EnemyAIController)))
	{
		return;
	}

	UBlackboardComponent* BlackboardComponent{ OwnerComp.GetBlackboardComponent() };
	if (!ensureAlways(IsValid(BlackboardComponent)))
	{
		return;
	}

	if (BlackboardComponent->GetValueAsBool(BlockFaceTargetBlackboardKey.SelectedKeyName))
	{
		return;
	}

	ANetherCrownEnemy* OwnerEnemy{ Cast<ANetherCrownEnemy>(EnemyAIController->GetPawn()) };
	if (!ensureAlways(IsValid(OwnerEnemy)) || !OwnerEnemy->HasAuthority())
	{
		return;
	}

	const UNetherCrownEnemyActionControlComponent* EnemyActionControlComponent{ OwnerEnemy->GetEnemyActionControlComponent() };
	if (!ensureAlways(IsValid(EnemyActionControlComponent)) || !EnemyActionControlComponent->CanFaceTarget())
	{
		return;
	}

	const AActor* TargetActor{ Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetActorBlackboardKey.SelectedKeyName)) };
	if (!IsValid(TargetActor))
	{
		return;
	}

	const FVector DirectionToTarget{ (TargetActor->GetActorLocation() - OwnerEnemy->GetActorLocation()).GetSafeNormal2D() };
	if (DirectionToTarget.IsNearlyZero())
	{
		return;
	}

	const FRotator TargetRotation{ 0.0f, DirectionToTarget.Rotation().Yaw, 0.0f };
	const FRotator CurrentRotation{ OwnerEnemy->GetActorRotation() };
	const FRotator NewRotation{ FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, RotationInterpSpeed) };

	EnemyAIController->SetControlRotation(NewRotation);
	OwnerEnemy->SetActorRotation(NewRotation);
}
