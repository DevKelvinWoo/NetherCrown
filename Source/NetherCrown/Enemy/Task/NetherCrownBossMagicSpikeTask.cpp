// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownBossMagicSpikeTask.h"

#include "NetherCrown/Enemy/NetherCrownBossEnemy.h"
#include "NetherCrown/Enemy/AIController/NetherCrownEnemyAIController.h"
#include "NetherCrown/Enemy/Components/NetherCrownEnemySkillComponent.h"
#include "NetherCrown/Enemy/EnemySkill/NetherCrownEnemySkillObject.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"

UNetherCrownBossMagicSpikeTask::UNetherCrownBossMagicSpikeTask()
{
	NodeName = TEXT("BossMagicSpike");
}

EBTNodeResult::Type UNetherCrownBossMagicSpikeTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	const ANetherCrownEnemyAIController* EnemyAIController{ Cast<ANetherCrownEnemyAIController>(OwnerComp.GetAIOwner()) };
	if (!ensureAlways(IsValid(EnemyAIController)))
	{
		return EBTNodeResult::Failed;
	}

	const ANetherCrownBossEnemy* OwnerBossEnemy{ Cast<ANetherCrownBossEnemy>(EnemyAIController->GetPawn()) };
	if (!ensureAlways(IsValid(OwnerBossEnemy)))
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
	MagicSpikeSkillObject->GetOnEnemySkillFinished().AddUObject(this, &ThisClass::HandleOnMagicSpikeSkillFinished);

	EnemySkillComponent->ActivateEnemySkill(NetherCrownTags::Enemy_Skill_MagicSpike);

	return EBTNodeResult::InProgress;
}

void UNetherCrownBossMagicSpikeTask::HandleOnMagicSpikeSkillFinished()
{
	UBehaviorTreeComponent* CachedOwnerComp{ CachedOwnerCompWeak.Get() };
	if (!ensureAlways(IsValid(CachedOwnerComp)))
	{
		CachedOwnerCompWeak.Reset();
		return;
	}

	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}
