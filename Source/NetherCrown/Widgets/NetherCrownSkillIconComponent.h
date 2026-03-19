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

UCLASS()
class NETHERCROWN_API UNetherCrownSkillIconComponent : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHandlingSkillObject(UNetherCrownSkillObject* InHandlingSkillObject);
	void SetSkillCoolDownProgress(float Percent);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText SkillKeyText{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Skill")
	ENetherCrownSkillKeyEnum SkillKeyEnum{ ENetherCrownSkillKeyEnum::None };

protected:
	virtual void NativeOnInitialized() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> NativeSkillThumbnailImage{};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UProgressBar> NativeSkillThumbnailProgressBar{};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> NativeSkillKeyText{};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMenuAnchor> NativeSkillTooltipAnchor{};

	UPROPERTY(EditAnywhere)
	TSubclassOf<UNetherCrownSkillTooltipView> SkillTooltipWidgetClass{};

private:
	void ApplySkillVisual();
	void TryResolveHandlingSkillObject();

	UFUNCTION()
	UUserWidget* HandleOnGetSkillTooltipWidgetInMenuAnchor();

	UPROPERTY(Transient)
	TObjectPtr<UNetherCrownSkillObject> HandlingSkillObject{};
};
