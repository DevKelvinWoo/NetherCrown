// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownBossCrownPrisonTask.h"

#include "NetherCrown/Enemy/NetherCrownBossEnemy.h"
#include "NetherCrown/Enemy/Components/NetherCrownEnemySkillComponent.h"
#include "NetherCrown/Enemy/EnemySkill/NetherCrownEnemySkillObject.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"

UNetherCrownBossCrownPrisonTask::UNetherCrownBossCrownPrisonTask()
{
	NodeName = TEXT("BossCrownPrison");
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UNetherCrownBossCrownPrisonTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	const ANetherCrownBossEnemy* OwnerBossEnemy{ GetControlledEnemy<ANetherCrownBossEnemy>(OwnerComp) };
	if (!IsValid(OwnerBossEnemy))
	{
		return EBTNodeResult::Failed;
	}

	UNetherCrownEnemySkillComponent* EnemySkillComponent{ OwnerBossEnemy->GetEnemySkillComponent() };
	if (!ensureAlways(IsValid(EnemySkillComponent)))
	{
		return EBTNodeResult::Failed;
	}

	CachedOwnerCompWeak = MakeWeakObjectPtr(&OwnerComp);

	UNetherCrownEnemySkillObject* CrownPrisonSkillObject{ EnemySkillComponent->GetEnemySkillObject(NetherCrownTags::Enemy_Skill_CrownPrison) };
	if (!ensureAlways(IsValid(CrownPrisonSkillObject)))
	{
		return EBTNodeResult::Failed;
	}
	CachedCrownPrisonSkillObjectWeak = MakeWeakObjectPtr(CrownPrisonSkillObject);
	CrownPrisonSkillObject->GetOnEnemySkillFinished().RemoveAll(this);
	CrownPrisonSkillObject->GetOnEnemySkillFinished().AddUObject(this, &ThisClass::HandleOnCrownPrisonSkillFinished);

	if (!EnemySkillComponent->ActivateEnemySkill(NetherCrownTags::Enemy_Skill_CrownPrison))
	{
		ResetTaskState();
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UNetherCrownBossCrownPrisonTask::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);

	ResetTaskState();

	return EBTNodeResult::Aborted;
}

void UNetherCrownBossCrownPrisonTask::HandleOnCrownPrisonSkillFinished()
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

void UNetherCrownBossCrownPrisonTask::ResetTaskState()
{
	if (UNetherCrownEnemySkillObject* CrownPrisonSkillObject{ CachedCrownPrisonSkillObjectWeak.Get() })
	{
		CrownPrisonSkillObject->GetOnEnemySkillFinished().RemoveAll(this);
	}

	CachedCrownPrisonSkillObjectWeak.Reset();
	CachedOwnerCompWeak.Reset();
}
