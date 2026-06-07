// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownBossMagicSpikeTask.h"

#include "NetherCrown/Enemy/NetherCrownBossEnemy.h"
#include "NetherCrown/Enemy/Components/NetherCrownEnemySkillComponent.h"
#include "NetherCrown/Enemy/EnemySkill/NetherCrownEnemySkillObject.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"

UNetherCrownBossMagicSpikeTask::UNetherCrownBossMagicSpikeTask()
{
	NodeName = TEXT("BossMagicSpike");
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UNetherCrownBossMagicSpikeTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	UNetherCrownEnemySkillObject* MagicSpikeSkillObject{ EnemySkillComponent->GetEnemySkillObject(NetherCrownTags::Enemy_Skill_MagicSpike) };
	if (!ensureAlways(IsValid(MagicSpikeSkillObject)))
	{
		return EBTNodeResult::Failed;
	}
	CachedMagicSpikeSkillObjectWeak = MakeWeakObjectPtr(MagicSpikeSkillObject);
	MagicSpikeSkillObject->GetOnEnemySkillFinished().RemoveAll(this);
	MagicSpikeSkillObject->GetOnEnemySkillFinished().AddUObject(this, &ThisClass::HandleOnMagicSpikeSkillFinished);

	if (!EnemySkillComponent->ActivateEnemySkill(NetherCrownTags::Enemy_Skill_MagicSpike))
	{
		ResetTaskState();
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UNetherCrownBossMagicSpikeTask::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);

	ResetTaskState();

	return EBTNodeResult::Aborted;
}

void UNetherCrownBossMagicSpikeTask::HandleOnMagicSpikeSkillFinished()
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

void UNetherCrownBossMagicSpikeTask::ResetTaskState()
{
	if (UNetherCrownEnemySkillObject* MagicSpikeSkillObject{ CachedMagicSpikeSkillObjectWeak.Get() })
	{
		MagicSpikeSkillObject->GetOnEnemySkillFinished().RemoveAll(this);
	}

	CachedMagicSpikeSkillObjectWeak.Reset();
	CachedOwnerCompWeak.Reset();
}
