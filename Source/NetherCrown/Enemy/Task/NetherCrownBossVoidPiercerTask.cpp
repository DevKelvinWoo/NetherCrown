// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownBossVoidPiercerTask.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NetherCrown/Enemy/NetherCrownBossEnemy.h"
#include "NetherCrown/Enemy/AIController/NetherCrownEnemyAIController.h"
#include "NetherCrown/Enemy/Components/NetherCrownEnemySkillComponent.h"
#include "NetherCrown/Enemy/EnemySkill/NetherCrownEnemyVoidPiercer.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"

UNetherCrownBossVoidPiercerTask::UNetherCrownBossVoidPiercerTask()
{
	NodeName = TEXT("BossVoidPiercer");

	BlockFaceTargetBlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, BlockFaceTargetBlackboardKey));
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

	UBlackboardComponent* BlackboardComponent{ OwnerComp.GetBlackboardComponent() };
	if (!ensureAlways(IsValid(BlackboardComponent)))
	{
		return EBTNodeResult::Failed;
	}

	BlackboardComponent->SetValueAsBool(BlockFaceTargetBlackboardKey.SelectedKeyName, true);

	VoidPiercerSkillObject->GetOnEnemySkillFinished().AddUObject(this, &ThisClass::HandleVoidPiercerFinished);

	if (!EnemySkillComponent->ActivateEnemySkill(NetherCrownTags::Enemy_Skill_VoidPiercer))
	{
		VoidPiercerSkillObject->GetOnEnemySkillFinished().RemoveAll(this);
		BlackboardComponent->SetValueAsBool(BlockFaceTargetBlackboardKey.SelectedKeyName, false);
		CachedOwnerCompWeak.Reset();
		return EBTNodeResult::Failed;
	}

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

	UBlackboardComponent* BlackboardComponent{ CachedOwnerComp->GetBlackboardComponent() };
	if (!ensureAlways(IsValid(BlackboardComponent)))
	{
		CachedOwnerCompWeak.Reset();
		return;
	}

	BlackboardComponent->SetValueAsBool(BlockFaceTargetBlackboardKey.SelectedKeyName, false);

	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}
