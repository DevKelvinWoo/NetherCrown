// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownBossCrownPrisonTask.h"

#include "NetherCrown/Enemy/NetherCrownBossEnemy.h"
#include "NetherCrown/Enemy/AIController/NetherCrownEnemyAIController.h"
#include "NetherCrown/Enemy/Components/NetherCrownEnemySkillComponent.h"
#include "NetherCrown/Enemy/EnemySkill/NetherCrownEnemySkillObject.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"

UNetherCrownBossCrownPrisonTask::UNetherCrownBossCrownPrisonTask()
{
	NodeName = TEXT("BossCrownPrison");
}

EBTNodeResult::Type UNetherCrownBossCrownPrisonTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	UNetherCrownEnemySkillObject* CrownPrisonSkillObject{ EnemySkillComponent->GetEnemySkillObject(NetherCrownTags::Enemy_Skill_CrownPrison) };
	if (!ensureAlways(IsValid(CrownPrisonSkillObject)))
	{
		return EBTNodeResult::Failed;
	}
	CrownPrisonSkillObject->GetOnEnemySkillFinished().AddUObject(this, &ThisClass::HandleOnCrownPrisonSkillFinished);

	EnemySkillComponent->ActivateEnemySkill(NetherCrownTags::Enemy_Skill_CrownPrison);

	return EBTNodeResult::InProgress;
}

void UNetherCrownBossCrownPrisonTask::HandleOnCrownPrisonSkillFinished()
{
	UBehaviorTreeComponent* CachedOwnerComp{ CachedOwnerCompWeak.Get() };
	if (!ensureAlways(IsValid(CachedOwnerComp)))
	{
		CachedOwnerCompWeak.Reset();
		return;
	}

	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}
