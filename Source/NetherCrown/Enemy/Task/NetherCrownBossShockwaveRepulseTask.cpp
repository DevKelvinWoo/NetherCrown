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
	bCreateNodeInstance = true;
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
	CachedShockwaveRepulseSkillObjectWeak = MakeWeakObjectPtr(ShockwaveRepulseSkillObject);
	ShockwaveRepulseSkillObject->GetOnEnemySkillFinished().RemoveAll(this);
	ShockwaveRepulseSkillObject->GetOnEnemySkillFinished().AddUObject(this, &ThisClass::HandleOnShockwaveRepulseSkillFinished);

	if (!OwnerBossEnemySkillComponent->ActivateEnemySkill(NetherCrownTags::Enemy_Skill_ShockwaveRepulse))
	{
		ResetTaskState();
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UNetherCrownBossShockwaveRepulseTask::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);

	ResetTaskState();

	return EBTNodeResult::Aborted;
}

void UNetherCrownBossShockwaveRepulseTask::HandleOnShockwaveRepulseSkillFinished()
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

void UNetherCrownBossShockwaveRepulseTask::ResetTaskState()
{
	if (UNetherCrownEnemySkillObject* ShockwaveRepulseSkillObject{ CachedShockwaveRepulseSkillObjectWeak.Get() })
	{
		ShockwaveRepulseSkillObject->GetOnEnemySkillFinished().RemoveAll(this);
	}

	CachedShockwaveRepulseSkillObjectWeak.Reset();
	CachedOwnerCompWeak.Reset();
}
