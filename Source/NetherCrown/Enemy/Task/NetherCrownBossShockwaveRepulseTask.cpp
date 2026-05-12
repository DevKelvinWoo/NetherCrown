// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownBossShockwaveRepulseTask.h"

#include "NetherCrown/Enemy/NetherCrownBossEnemy.h"
#include "NetherCrown/Enemy/AIController/NetherCrownEnemyAIController.h"
#include "NetherCrown/Enemy/Components/NetherCrownEnemySkillComponent.h"
#include "NetherCrown/Enemy/EnemySkill/NetherCrownEnemySkillObject.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"

UNetherCrownBossShockwaveRepulseTask::UNetherCrownBossShockwaveRepulseTask()
{
	NodeName = TEXT("Boss Shockwave Repulse");
}

EBTNodeResult::Type UNetherCrownBossShockwaveRepulseTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	UNetherCrownEnemySkillComponent* OwnerBossEnemySkillComponent{ OwnerBossEnemy->GetEnemySkillComponent() };
	if (!ensureAlways(IsValid(OwnerBossEnemySkillComponent)))
	{
		return EBTNodeResult::Failed;
	}

	CachedOwnerCompWeak = MakeWeakObjectPtr(&OwnerComp);

	UNetherCrownEnemySkillObject* ShockwaveRepulseSkillObject{ OwnerBossEnemySkillComponent->GetEnemySkillObject(NetherCrownTags::Enemy_Skill_ShockwaveRepulse) };
	if (!ensureAlways(IsValid(ShockwaveRepulseSkillObject)))
	{
		return EBTNodeResult::Failed;
	}
	ShockwaveRepulseSkillObject->GetOnEnemySkillFinished().AddUObject(this, &ThisClass::HandleOnShockwaveRepulseSkillFinished);

	if (!OwnerBossEnemySkillComponent->ActivateEnemySkill(NetherCrownTags::Enemy_Skill_ShockwaveRepulse))
	{
		ShockwaveRepulseSkillObject->GetOnEnemySkillFinished().RemoveAll(this);
		CachedOwnerCompWeak.Reset();
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}

void UNetherCrownBossShockwaveRepulseTask::HandleOnShockwaveRepulseSkillFinished()
{
	UBehaviorTreeComponent* CachedOwnerComp{ CachedOwnerCompWeak.Get() };
	if (!ensureAlways(IsValid(CachedOwnerComp)))
	{
		CachedOwnerCompWeak.Reset();
		return;
	}

	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}
