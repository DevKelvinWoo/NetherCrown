// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrown/UI/NetherCrownUIScreenBase.h"
#include "NetherCrownSkillTooltipView.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class NETHERCROWN_API UNetherCrownSkillTooltipView : public UNetherCrownUIScreenBase
{
	GENERATED_BODY()

public:
	void SetSkillTooltipText(const FText& Text);
	void SetSkillThumbnailImage(const TSoftObjectPtr<UTexture2D> SoftTexture);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NativeSkillToolTipText{};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> NativeSkillThumbnailImage{};
};
