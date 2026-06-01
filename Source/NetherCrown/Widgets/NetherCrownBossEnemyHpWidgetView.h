// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrown/UI/NetherCrownUIScreenBase.h"
#include "NetherCrownBossEnemyHpWidgetView.generated.h"

class UNetherCrownSliderComponent;
class ANetherCrownEnemy;
class UNetherCrownEnemyStatComponent;

UCLASS()
class NETHERCROWN_API UNetherCrownBossEnemyHpWidgetView : public UNetherCrownUIScreenBase
{
	GENERATED_BODY()

public:
	void InitNormalEnemyHPWidget(ANetherCrownEnemy* Enemy);
	void SetBossEnemyHPSliderValue(const float Percent);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

private:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UNetherCrownSliderComponent> BossEnemyHPSlider{};

	TWeakObjectPtr<UNetherCrownEnemyStatComponent> BoundEnemyStatComponentWeak{};
};
