// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownEnemyBTServiceBase.h"
#include "NetherCrownBossEnemy1EscapeSkillDecisionService.generated.h"

class UNetherCrownEnemySkillComponent;

UCLASS()
class NETHERCROWN_API UNetherCrownBossEnemy1EscapeSkillDecisionService : public UNetherCrownEnemyBTServiceBase
{
	GENERATED_BODY()

public:
	UNetherCrownBossEnemy1EscapeSkillDecisionService();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, const float DeltaSeconds) override;

private:
	bool IsEnemySkillCoolDown(const FGameplayTag& InSkillTag) const;

	bool CanUseTeleportSkill() const;
	bool CanUseShockwaveRepulseSkill() const;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector SelectedEscapeSkillKeyNameBlackboardKey{};

	TArray<FGameplayTag> AvailableEscapeSkillGameplayTags{};
	FGameplayTag SelectedEscapeSkillGameplayTag{};

	TWeakObjectPtr<UNetherCrownEnemySkillComponent> BossEnemySkillComponentWeak{};
};
