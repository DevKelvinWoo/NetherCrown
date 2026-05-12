// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownEnemyTeleportTask.h"

#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Enemy/AIController/NetherCrownEnemyAIController.h"
#include "NetherCrown/Enemy/Components/NetherCrownEnemySkillComponent.h"
#include "NetherCrown/Enemy/EnemySkill/NetherCrownEnemySkillObject.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"

UNetherCrownEnemyTeleportTask::UNetherCrownEnemyTeleportTask()
{
	NodeName = TEXT("Teleport");
}

EBTNodeResult::Type UNetherCrownEnemyTeleportTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	TeleportSkillObject->GetOnEnemySkillFinished().AddUObject(this, &ThisClass::HandleOnTeleportSkillFinished);

	if (!EnemySkillComponent->ActivateEnemySkill(NetherCrownTags::Enemy_Skill_Teleport))
	{
		TeleportSkillObject->GetOnEnemySkillFinished().RemoveAll(this);
		CachedOwnerCompWeak.Reset();
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}

void UNetherCrownEnemyTeleportTask::HandleOnTeleportSkillFinished()
{
	UBehaviorTreeComponent* CachedOwnerComp{ CachedOwnerCompWeak.Get() };
	if (!ensureAlways(IsValid(CachedOwnerComp)))
	{
		CachedOwnerCompWeak.Reset();
		return;
	}

	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}
