// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownEnemyTeleportTask.h"

#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Enemy/Components/NetherCrownEnemySkillComponent.h"
#include "NetherCrown/Enemy/EnemySkill/NetherCrownEnemySkillObject.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"

UNetherCrownEnemyTeleportTask::UNetherCrownEnemyTeleportTask()
{
	NodeName = TEXT("Teleport");
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UNetherCrownEnemyTeleportTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	const ANetherCrownEnemy* OwnerEnemy{ GetControlledEnemy(OwnerComp) };
	if (!IsValid(OwnerEnemy))
	{
		return EBTNodeResult::Failed;
	}

	UNetherCrownEnemySkillComponent* EnemySkillComponent{ OwnerEnemy->GetEnemySkillComponent() };
	if (!ensureAlways(IsValid(EnemySkillComponent)))
	{
		return EBTNodeResult::Failed;
	}

	CachedOwnerCompWeak = MakeWeakObjectPtr(&OwnerComp);

	UNetherCrownEnemySkillObject* TeleportSkillObject{ EnemySkillComponent->GetEnemySkillObject(NetherCrownTags::Enemy_Skill_Teleport) };
	if (!ensureAlways(IsValid(TeleportSkillObject)))
	{
		return EBTNodeResult::Failed;
	}
	CachedTeleportSkillObjectWeak = MakeWeakObjectPtr(TeleportSkillObject);
	TeleportSkillObject->GetOnEnemySkillFinished().RemoveAll(this);
	TeleportSkillObject->GetOnEnemySkillFinished().AddUObject(this, &ThisClass::HandleOnTeleportSkillFinished);

	if (!EnemySkillComponent->ActivateEnemySkill(NetherCrownTags::Enemy_Skill_Teleport))
	{
		ResetTaskState();
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UNetherCrownEnemyTeleportTask::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);

	ResetTaskState();

	return EBTNodeResult::Aborted;
}

void UNetherCrownEnemyTeleportTask::HandleOnTeleportSkillFinished()
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

void UNetherCrownEnemyTeleportTask::ResetTaskState()
{
	if (UNetherCrownEnemySkillObject* TeleportSkillObject{ CachedTeleportSkillObjectWeak.Get() })
	{
		TeleportSkillObject->GetOnEnemySkillFinished().RemoveAll(this);
	}

	CachedTeleportSkillObjectWeak.Reset();
	CachedOwnerCompWeak.Reset();
}
