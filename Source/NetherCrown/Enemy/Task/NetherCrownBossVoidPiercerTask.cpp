// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownBossVoidPiercerTask.h"

#include "NetherCrown/Enemy/NetherCrownBossEnemy.h"
#include "NetherCrown/Enemy/AIController/NetherCrownEnemyAIController.h"
#include "NetherCrown/Enemy/Components/NetherCrownEnemySkillComponent.h"
#include "NetherCrown/Enemy/EnemySkill/NetherCrownEnemyVoidPiercer.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"

UNetherCrownBossVoidPiercerTask::UNetherCrownBossVoidPiercerTask()
{
	NodeName = TEXT("BossVoidPiercer");
}

EBTNodeResult::Type UNetherCrownBossVoidPiercerTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	const ANetherCrownEnemyAIController* EnemyAIController{ Cast<ANetherCrownEnemyAIController>(OwnerComp.GetAIOwner()) };
	if (!ensureAlways(IsValid(EnemyAIController)))
	{
		return EBTNodeResult::Failed;
	}

	const ANetherCrownBossEnemy* BossEnemy{ Cast<ANetherCrownBossEnemy>(EnemyAIController->GetPawn()) };
	if (!ensureAlways(IsValid(BossEnemy)))
	{
		return EBTNodeResult::Failed;
	}

	UNetherCrownEnemySkillComponent* EnemySkillComponent{ BossEnemy->GetEnemySkillComponent() };
	if (!ensureAlways(IsValid(EnemySkillComponent)))
	{
		return EBTNodeResult::Failed;
	}

	CachedOwnerCompWeak = MakeWeakObjectPtr(&OwnerComp);

	UNetherCrownEnemyVoidPiercer* VoidPiercerSkillObject{ Cast<UNetherCrownEnemyVoidPiercer>(EnemySkillComponent->GetEnemySkillObject(NetherCrownTags::Enemy_Skill_VoidPiercer)) };
	if (!ensureAlways(IsValid(VoidPiercerSkillObject)))
	{
		return EBTNodeResult::Failed;
	}

	VoidPiercerSkillObject->GetOnEnemySkillFinished().AddUObject(this, &ThisClass::HandleVoidPiercerFinished);

	EnemySkillComponent->ActivateEnemySkill(NetherCrownTags::Enemy_Skill_VoidPiercer);

	return EBTNodeResult::InProgress;
}

void UNetherCrownBossVoidPiercerTask::HandleVoidPiercerFinished()
{
	UBehaviorTreeComponent* CachedOwnerComp{ CachedOwnerCompWeak.Get() };
	if (!ensureAlways(IsValid(CachedOwnerComp)))
	{
		CachedOwnerCompWeak.Reset();
		return;
	}

	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}
