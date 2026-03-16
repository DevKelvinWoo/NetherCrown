// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NetherCrownSkillIconComponent.generated.h"

class UProgressBar;
class UImage;
class UTextBlock;

UCLASS()
class NETHERCROWN_API UNetherCrownSkillIconComponent : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetSkillCoolDownProgress(float Percent);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UTexture2D> SkillThumbnailTexture{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText SkillKeyText{};

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> NativeSkillThumbnailImage{};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UProgressBar> NativeSkillThumbnailProgressBar{};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> NativeSkillKeyText{};
};
