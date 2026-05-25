// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownBossEnemyHpWidgetView.h"

#include "NetherCrownSliderComponent.h"
#include "NetherCrown/Components/NetherCrownEnemyStatComponent.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"

void UNetherCrownBossEnemyHpWidgetView::InitNormalEnemyHPWidget(ANetherCrownEnemy* Enemy)
{
	if (!ensureAlways(IsValid(Enemy)))
	{
		return;
	}

	UNetherCrownEnemyStatComponent* EnemyStatComponent{ Enemy->GetEnemyStatComponent() };
	if (!ensureAlways(IsValid(EnemyStatComponent)))
	{
		return;
	}

	EnemyStatComponent->GetOnEnemyHPModified().AddUObject(this, &ThisClass::SetBossEnemyHPSliderValue);
	SetBossEnemyHPSliderValue(1.f);
}

void UNetherCrownBossEnemyHpWidgetView::SetBossEnemyHPSliderValue(const float Percent)
{
	if (!ensureAlways(IsValid(BossEnemyHPSlider)))
	{
		return;
	}

	BossEnemyHPSlider->SetProgress(Percent);
}

void UNetherCrownBossEnemyHpWidgetView::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}
