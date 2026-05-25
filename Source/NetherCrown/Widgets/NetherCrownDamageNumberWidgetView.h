// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NetherCrownDamageNumberWidgetView.generated.h"

class UTextBlock;
class UWidgetAnimation;

UCLASS()
class NETHERCROWN_API UNetherCrownDamageNumberWidgetView : public UUserWidget
{
	GENERATED_BODY()
	DECLARE_MULTICAST_DELEGATE(FOnDamageNumberAnimationFinished);

public:
	void InitDamageNumber(const int32 DamageAmount, const bool bIsCriticalDamage);
	void PlayDamageNumberAnimation();

	FOnDamageNumberAnimationFinished& GetOnDamageNumberAnimationFinished() { return OnDamageNumberAnimationFinished; }

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void HandleDamageNumberAnimationFinished();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DamageNumberText{};

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> DamageNumberAnimation{};

	UPROPERTY(EditDefaultsOnly, Category = "DamageNumber")
	FLinearColor NormalDamageColor{ FLinearColor::White };

	UPROPERTY(EditDefaultsOnly, Category = "DamageNumber")
	FLinearColor CriticalDamageColor{ FLinearColor(1.f, 0.75f, 0.f, 1.f) };

	FOnDamageNumberAnimationFinished OnDamageNumberAnimationFinished{};
};
