// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownBossEnemy1EscapeSkillDecisionService.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NetherCrown/Enemy/NetherCrownBossEnemy.h"
#include "NetherCrown/Enemy/AIController/NetherCrownEnemyAIController.h"
#include "NetherCrown/Enemy/Components/NetherCrownEnemySkillComponent.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"

UNetherCrownBossEnemy1EscapeSkillDecisionService::UNetherCrownBossEnemy1EscapeSkillDecisionService()
{
	NodeName = TEXT("Escape Skill Decision Service");

	SelectedEscapeSkillKeyNameBlackboardKey.AddNameFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, SelectedEscapeSkillKeyNameBlackboardKey));
}

void UNetherCrownBossEnemy1EscapeSkillDecisionService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, const float DeltaSeconds)
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
	BlackboardComponent->SetValueAsName(SelectedEscapeSkillKeyNameBlackboardKey.SelectedKeyName, TEXT(""));

	BossEnemySkillComponentWeak = MakeWeakObjectPtr(OwnerBossEnemy->GetEnemySkillComponent());
	if (!BossEnemySkillComponentWeak.IsValid() || !BossEnemySkillComponentWeak->CanActivateEnemySkill())
	{
		return;
	}

	AvailableEscapeSkillGameplayTags.Empty();
	SelectedEscapeSkillGameplayTag = FGameplayTag();

	if (CanUseTeleportSkill())
	{
		AvailableEscapeSkillGameplayTags.Add(NetherCrownTags::Enemy_Skill_Teleport);
	}

	if (CanUseShockwaveRepulseSkill())
	{
		AvailableEscapeSkillGameplayTags.Add(NetherCrownTags::Enemy_Skill_ShockwaveRepulse);
	}

	const int32 AvailableEscapeSkillCount{ AvailableEscapeSkillGameplayTags.Num() };
	if (AvailableEscapeSkillCount == 0)
	{
		return;
	}

	SelectedEscapeSkillGameplayTag = AvailableEscapeSkillGameplayTags[FMath::RandRange(0, AvailableEscapeSkillCount - 1)];

	BlackboardComponent->SetValueAsName(SelectedEscapeSkillKeyNameBlackboardKey.SelectedKeyName, SelectedEscapeSkillGameplayTag.GetTagName());
}

bool UNetherCrownBossEnemy1EscapeSkillDecisionService::IsEnemySkillCoolDown(const FGameplayTag& InSkillTag) const
{
	const UNetherCrownEnemySkillComponent* BossEnemySkillComponent{ BossEnemySkillComponentWeak.Get() };
	if (!ensureAlways(IsValid(BossEnemySkillComponent)))
	{
		return false;
	}

	return BossEnemySkillComponent->IsEnemySkillCoolDown(InSkillTag);
}

bool UNetherCrownBossEnemy1EscapeSkillDecisionService::CanUseTeleportSkill() const
{
	const bool IsTeleportSkillCoolDown{ IsEnemySkillCoolDown(NetherCrownTags::Enemy_Skill_Teleport) };

	return !IsTeleportSkillCoolDown;
}

bool UNetherCrownBossEnemy1EscapeSkillDecisionService::CanUseShockwaveRepulseSkill() const
{
	const bool IsShockwaveRepulseSkillCoolDown{ IsEnemySkillCoolDown(NetherCrownTags::Enemy_Skill_ShockwaveRepulse) };

	return !IsShockwaveRepulseSkillCoolDown;
}
