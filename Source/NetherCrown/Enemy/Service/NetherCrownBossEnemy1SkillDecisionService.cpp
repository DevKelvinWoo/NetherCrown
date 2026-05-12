// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownBossEnemy1SkillDecisionService.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NetherCrown/Enemy/NetherCrownBossEnemy.h"
#include "NetherCrown/Enemy/AIController/NetherCrownEnemyAIController.h"
#include "NetherCrown/Enemy/Components/NetherCrownEnemySkillComponent.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"

UNetherCrownBossEnemy1SkillDecisionService::UNetherCrownBossEnemy1SkillDecisionService()
{
	NodeName = TEXT("Skill Decision");

	SelectedSkillKeyNameBlackboardKey.AddNameFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, SelectedSkillKeyNameBlackboardKey));
}

void UNetherCrownBossEnemy1SkillDecisionService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const ANetherCrownEnemyAIController* EnemyAIController{ Cast<ANetherCrownEnemyAIController>(OwnerComp.GetAIOwner()) };
	if (!ensureAlways(IsValid(EnemyAIController)))
	{
		return;
	}

	const ANetherCrownBossEnemy* OwnerBossEnemy{ Cast<ANetherCrownBossEnemy>(EnemyAIController->GetPawn()) };
	if (!ensureAlways(IsValid(OwnerBossEnemy)))
	{
		return;
	}

	UBlackboardComponent* BlackboardComponent{ OwnerComp.GetBlackboardComponent() };
	if (!ensureAlways(IsValid(BlackboardComponent)))
	{
		return;
	}
	BlackboardComponent->SetValueAsName(SelectedSkillKeyNameBlackboardKey.SelectedKeyName, TEXT(""));

	BossEnemySkillComponentWeak = MakeWeakObjectPtr(OwnerBossEnemy->GetEnemySkillComponent());

	AvailableSkillGameplayTags.Empty();
	SelectedSkillGameplayTag = FGameplayTag();

	if (CanUseVoidPiercerSkill())
	{
		AvailableSkillGameplayTags.Add(NetherCrownTags::Enemy_Skill_VoidPiercer);
	}

	if (CanUseCrownPrisonSkill())
	{
		AvailableSkillGameplayTags.Add(NetherCrownTags::Enemy_Skill_CrownPrison);
	}

	if (CanUseMagicSpikeSkill())
	{
		AvailableSkillGameplayTags.Add(NetherCrownTags::Enemy_Skill_MagicSpike);
	}

	const int32 AvailableSkillCount{ AvailableSkillGameplayTags.Num() };
	if (AvailableSkillCount == 0)
	{
		return;
	}

	SelectedSkillGameplayTag = AvailableSkillGameplayTags[FMath::RandRange(0, AvailableSkillCount - 1)];

	BlackboardComponent->SetValueAsName(SelectedSkillKeyNameBlackboardKey.SelectedKeyName, SelectedSkillGameplayTag.GetTagName());
}

bool UNetherCrownBossEnemy1SkillDecisionService::IsEnemySkillCoolDown(const FGameplayTag& InSkillTag) const
{
	const UNetherCrownEnemySkillComponent* BossEnemySkillComponent{ BossEnemySkillComponentWeak.Get() };
	if (!ensureAlways(IsValid(BossEnemySkillComponent)))
	{
		return false;
	}

	return BossEnemySkillComponent->IsEnemySkillCoolDown(InSkillTag);
}

bool UNetherCrownBossEnemy1SkillDecisionService::CanUseVoidPiercerSkill() const
{
	const bool IsVoidPiercerSkillCoolDown{ IsEnemySkillCoolDown(NetherCrownTags::Enemy_Skill_VoidPiercer) };

	return !IsVoidPiercerSkillCoolDown;
}

bool UNetherCrownBossEnemy1SkillDecisionService::CanUseCrownPrisonSkill() const
{
	const bool IsCrownPrisonSkillCoolDown{ IsEnemySkillCoolDown(NetherCrownTags::Enemy_Skill_CrownPrison) };

	return !IsCrownPrisonSkillCoolDown;
}

bool UNetherCrownBossEnemy1SkillDecisionService::CanUseMagicSpikeSkill() const
{
	const bool IsMagicSpikeSkillCoolDown{ IsEnemySkillCoolDown(NetherCrownTags::Enemy_Skill_MagicSpike) };

	return !IsMagicSpikeSkillCoolDown;
}
