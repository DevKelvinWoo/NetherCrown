// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "NetherCrownDefaultSettings.generated.h"

class UDataTable;
class UCurveFloat;

class UNetherCrownLoadingScreenWidgetView;
class UNetherCrownPrimaryLayout;

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "NetherCrown Default Settings"))
class NETHERCROWN_API UNetherCrownDefaultSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	const TSoftObjectPtr<UDataTable>& GetCharacterSoundDT() const { return CharacterSoundDT; }
	const TSoftObjectPtr<UDataTable>& GetWeaponDT() const { return WeaponDT; }
	const TSoftObjectPtr<UDataTable>& GetSkillDT() const { return SkillDT; }
	const TSoftObjectPtr<UDataTable>& GetEnemySkillDT() const { return EnemySkillDT; }
	const TSoftObjectPtr<UDataTable>& GetEffectDT() const { return EffectDT; }
	const TSoftObjectPtr<UDataTable>& GetScreenDefinitionDT() const { return ScreenDefinitionDT; }
	const TSoftObjectPtr<UDataTable>& GetLevelSequenceDT() const { return LevelSequenceDT; }
	const TSoftObjectPtr<UDataTable>& GetLevelTravelDT() const { return LevelTravelDT; }
	const TSoftObjectPtr<UDataTable>& GetQuestDT() const { return QuestDT; }
	const TSubclassOf<UNetherCrownPrimaryLayout>& GetPrimaryLayoutWidgetClass() const { return PrimaryLayoutWidgetClass; }
	const TSubclassOf<UNetherCrownLoadingScreenWidgetView>& GetLoadingScreenWidgetClass() const { return LoadingScreenWidgetClass; }

private:
	UPROPERTY(Config, EditAnywhere, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UDataTable> CharacterSoundDT;

	UPROPERTY(Config, EditAnywhere, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UDataTable> WeaponDT;

	UPROPERTY(Config, EditAnywhere, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UDataTable> SkillDT;

	UPROPERTY(Config, EditAnywhere, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UDataTable> EnemySkillDT;

	UPROPERTY(Config, EditAnywhere, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UDataTable> EffectDT;

	UPROPERTY(Config, EditAnywhere, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UDataTable> ScreenDefinitionDT;

	UPROPERTY(Config, EditAnywhere, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UDataTable> LevelSequenceDT;

	UPROPERTY(Config, EditAnywhere, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UDataTable> LevelTravelDT;

	UPROPERTY(Config, EditAnywhere, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UDataTable> QuestDT;

	UPROPERTY(Config, EditAnywhere, Category = "MaterialParam", meta = (AllowPrivateAccess = "true"))
	FName FrozenTempestTargetMaterialParam{ TEXT("_VfxMix") };

	UPROPERTY(Config, EditAnywhere, Category = "MaterialParam", meta = (AllowPrivateAccess = "true"))
	FName SkyFallSlashArmMaterialParam{ TEXT("SkyFallSlashAlpha") };

	UPROPERTY(Config, EditAnywhere, Category = "MaterialAlpha", meta = (AllowPrivateAccess = "true"))
	float FrozenTempestTargetMaterialAlpha{ 0.85f };

	UPROPERTY(Config, EditAnywhere, Category = "Curve", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCurveFloat> TargetOverlayMaterialEndCurveFloatSoft{};

	UPROPERTY(Config, EditAnywhere, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UNetherCrownPrimaryLayout> PrimaryLayoutWidgetClass{};

	UPROPERTY(Config, EditAnywhere, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UNetherCrownLoadingScreenWidgetView> LoadingScreenWidgetClass{};
};
