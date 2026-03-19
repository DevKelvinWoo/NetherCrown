// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "NetherCrown/Skill/NetherCrownSkillObject.h"
#include "NetherCrownSkillData.generated.h"

UCLASS()
class NETHERCROWN_API UNetherCrownSkillDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const FNetherCrownSkillData& GetSkillData() const { return SkillData; }

private:
	UPROPERTY(EditAnywhere, Category = "SkillData")
	FNetherCrownSkillData SkillData{};
};

USTRUCT(BlueprintType)
struct FNetherCrownSkillDataTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	const FGameplayTag& GetSkillTag() const { return SkillTag; }
	TSoftObjectPtr<UNetherCrownSkillDataAsset> GetSkillDataAsset() const { return SkillDataAsset; }

private:
	UPROPERTY(EditAnywhere, Category = "SkillTag")
	FGameplayTag SkillTag{};

	UPROPERTY(EditAnywhere, Category = "SkillData")
	TSoftObjectPtr<UNetherCrownSkillDataAsset> SkillDataAsset{};
};
