// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownSkillEnemyDashAttack.h"

void UNetherCrownSkillEnemyDashAttack::EnemyDashAttack(const ANetherCrownCharacter* TargetCharacter)
{

}

void UNetherCrownSkillEnemyDashAttack::PlayEnemySkillCosmetics()
{
	Super::PlayEnemySkillCosmetics();

	UE_LOG(LogTemp, Warning, TEXT("Play Enemy SkillCosmetics!!!"));
}

void UNetherCrownSkillEnemyDashAttack::ExecuteEnemySkillGameplay()
{
	Super::ExecuteEnemySkillGameplay();

	UE_LOG(LogTemp, Warning, TEXT("Execute Enemy SkillGameplay!!!"));
}
