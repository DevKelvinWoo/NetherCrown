// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownEnemyDashAttackTask.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Enemy/AIController/NetherCrownEnemyAIController.h"
#include "NetherCrown/Enemy/Components/NetherCrownEnemySkillComponent.h"
#include "NetherCrown/Enemy/EnemySkill/NetherCrownSkillEnemyDashAttack.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"

UNetherCrownEnemyDashAttackTask::UNetherCrownEnemyDashAttackTask()
{
	NodeName = TEXT("Enemy Dash Attack");
	bCreateNodeInstance = true;

	NeedDashAttackBlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, NeedDashAttackBlackboardKey));
}

EBTNodeResult::Type UNetherCrownEnemyDashAttackTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	if (!ensureAlways(IsValid(OwnerEnemy->GetCurrentTargetCharacter())))
	{
		return EBTNodeResult::Failed;
	}

	UNetherCrownEnemySkillComponent* EnemySkillComponent{ OwnerEnemy->GetEnemySkillComponent() };
	if (!ensureAlways(IsValid(EnemySkillComponent)))
	{
		return EBTNodeResult::Failed;
	}

	UNetherCrownSkillEnemyDashAttack* DashAttackSkill{ Cast<UNetherCrownSkillEnemyDashAttack>(EnemySkillComponent->GetEnemySkillObject(NetherCrownTags::Enemy_Skill_DashAttack)) };
	if (!ensureAlways(IsValid(DashAttackSkill)))
	{
		return EBTNodeResult::Failed;
	}

	CachedOwnerCompWeak = MakeWeakObjectPtr(&OwnerComp);
	CachedDashAttackSkillWeak = MakeWeakObjectPtr(DashAttackSkill);
	DashAttackFinishedDelegateHandle = DashAttackSkill->GetOnEnemyDashAttackFinished().AddUObject(this, &ThisClass::HandleDashAttackFinished);

	EnemySkillComponent->ActivateEnemySkill(NetherCrownTags::Enemy_Skill_DashAttack);

	UBlackboardComponent* BlackboardComponent{ OwnerComp.GetBlackboardComponent() };
	if (!ensureAlways(IsValid(BlackboardComponent)))
	{
		ResetTaskState();
		return EBTNodeResult::Failed;
	}

	BlackboardComponent->SetValueAsBool(NeedDashAttackBlackboardKey.SelectedKeyName, false);

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UNetherCrownEnemyDashAttackTask::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);

	ResetTaskState();

	return EBTNodeResult::Aborted;
}

void UNetherCrownEnemyDashAttackTask::HandleDashAttackFinished()
{
	UBehaviorTreeComponent* CachedOwnerComp{ CachedOwnerCompWeak.Get() };
	if (!ensureAlways(IsValid(CachedOwnerComp)))
	{
		ResetTaskState();
		return;
	}

	ResetTaskState();
	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}

void UNetherCrownEnemyDashAttackTask::ResetTaskState()
{
	UNetherCrownSkillEnemyDashAttack* DashAttackSkill{ CachedDashAttackSkillWeak.Get() };
	if (IsValid(DashAttackSkill) && DashAttackFinishedDelegateHandle.IsValid())
	{
		DashAttackSkill->GetOnEnemyDashAttackFinished().Remove(DashAttackFinishedDelegateHandle);
	}

	DashAttackFinishedDelegateHandle.Reset();
	CachedDashAttackSkillWeak.Reset();
	CachedOwnerCompWeak.Reset();
}
