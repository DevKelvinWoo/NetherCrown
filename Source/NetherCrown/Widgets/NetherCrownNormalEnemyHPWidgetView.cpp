// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownNormalEnemyHPWidgetView.h"

#include "NetherCrownSliderComponent.h"
#include "NetherCrown/Components/NetherCrownEnemyStatComponent.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"

void UNetherCrownNormalEnemyHPWidgetView::InitNormalEnemyHPWidget(ANetherCrownEnemy* Enemy)
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

	EnemyStatComponent->GetOnEnemyHPModified().AddUObject(this, &ThisClass::SetEnemyHPSliderValue);
	SetEnemyHPSliderValue(1.f);
}

void UNetherCrownNormalEnemyHPWidgetView::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UNetherCrownNormalEnemyHPWidgetView::SetEnemyHPSliderValue(const float HpRatio)
{
	if (!ensureAlways(IsValid(HPSliderComponent)))
	{
		return;
	}

	HPSliderComponent->SetProgress(HpRatio);
}
