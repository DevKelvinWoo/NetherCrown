// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NetherCrown/Skill/NetherCrownSkillObject.h"
#include "NetherCrownSkillIconComponent.generated.h"

class UNetherCrownSkillTooltipView;
class UProgressBar;
class UImage;
class UTextBlock;
class UMenuAnchor;
class UWidgetAnimation;

UCLASS()
class NETHERCROWN_API UNetherCrownSkillIconComponent : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHandlingSkillObject(UNetherCrownSkillObject* InHandlingSkillObject);
	void SetSkillCoolDownProgress(const float Percent);
	void SetSkillIconActive(const bool bActive, const bool bPlayActivateAnimation = false);

protected:
	virtual void NativeOnInitialized() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

private:
	void ApplySkillVisual();
	void TryResolveHandlingSkillObject();

	UFUNCTION()
	UUserWidget* HandleOnGetSkillTooltipWidgetInMenuAnchor();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FText SkillKeyText{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Skill", meta = (AllowPrivateAccess = "true"))
	ENetherCrownSkillKeyEnum SkillKeyEnum{ ENetherCrownSkillKeyEnum::None };

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UImage> NativeSkillThumbnailImage{};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UProgressBar> NativeSkillThumbnailProgressBar{};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> NativeSkillKeyText{};

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UMenuAnchor> NativeSkillTooltipAnchor{};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UNetherCrownSkillTooltipView> SkillTooltipWidgetClass{};

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetAnimation> SkillIconActivateAnimation{};

	UPROPERTY(Transient)
	TObjectPtr<UNetherCrownSkillObject> HandlingSkillObject{};
};
