// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NetherCrownNormalEnemyHPWidgetView.generated.h"

class ANetherCrownEnemy;
class UNetherCrownEnemyStatComponent;
class UNetherCrownNormalEnemyHPWidgetViewModel;
class UNetherCrownSliderComponent;

UCLASS()
class NETHERCROWN_API UNetherCrownNormalEnemyHPWidgetView : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitNormalEnemyHPWidget(ANetherCrownEnemy* Enemy);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

private:
	void SetEnemyHPSliderValue(const float HpRatio);

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UNetherCrownSliderComponent> HPSliderComponent{};

	TWeakObjectPtr<UNetherCrownEnemyStatComponent> BoundEnemyStatComponentWeak{};
};
