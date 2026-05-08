// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "NetherCrownBossEnemy1SkillDecisionService.generated.h"

class UNetherCrownEnemySkillComponent;

UCLASS()
class NETHERCROWN_API UNetherCrownBossEnemy1SkillDecisionService : public UBTService
{
	GENERATED_BODY()

public:
	UNetherCrownBossEnemy1SkillDecisionService();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool IsEnemySkillCoolDown(const FGameplayTag& InSkillTag) const;

	bool CanUseVoidPiercerSkill() const;
	bool CanUseCrownPrisonSkill() const;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector SelectedSkillKeyNameBlackboardKey{};

	TArray<FGameplayTag> AvailableSkillGameplayTags{};
	FGameplayTag SelectedSkillGameplayTag{};

	TWeakObjectPtr<UNetherCrownEnemySkillComponent> BossEnemySkillComponentWeak{};
};
