// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "NetherCrownUITypes.h"
#include "NetherCrownUIManagerSubsystem.generated.h"

class UNetherCrownPrimaryLayout;
class UNetherCrownUIScreenBase;

UCLASS()
class NETHERCROWN_API UNetherCrownUIManagerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetPrimaryLayoutClass(TSubclassOf<UNetherCrownPrimaryLayout> InPrimaryLayoutClass);

	UFUNCTION(BlueprintCallable)
	void RegisterScreenDefinition(const FNetherCrownUIScreenDefinition& ScreenDefinition);

	UFUNCTION(BlueprintCallable)
	void RegisterScreenDefinitions(const TArray<FNetherCrownUIScreenDefinition>& InScreenDefinitions);

	UFUNCTION(BlueprintCallable)
	UNetherCrownPrimaryLayout* InitializePrimaryLayout();

	UFUNCTION(BlueprintCallable)
	UNetherCrownUIScreenBase* ShowScreenByTag(const FGameplayTag& ScreenTag);

	UFUNCTION(BlueprintCallable)
	void HideScreenByTag(const FGameplayTag& ScreenTag);

	UFUNCTION(BlueprintCallable)
	void ToggleScreenByTag(const FGameplayTag& ScreenTag);

	UFUNCTION(BlueprintCallable)
	bool IsScreenActive(const FGameplayTag& ScreenTag) const;

	UFUNCTION(BlueprintCallable)
	UNetherCrownUIScreenBase* GetScreenByTag(const FGameplayTag& ScreenTag) const;

	UFUNCTION(BlueprintCallable)
	void ClearLayer(const FGameplayTag& LayerTag);

protected:
	virtual void Deinitialize() override;

private:
	void AddScreenToLayerContainer(UNetherCrownUIScreenBase* ScreenWidget, const FGameplayTag& LayerTag);
	void RemoveScreenFromLayerContainer(UNetherCrownUIScreenBase* ScreenWidget, const FGameplayTag& LayerTag);
	TArray<TObjectPtr<UNetherCrownUIScreenBase>>& GetLayerContainer(const FGameplayTag& LayerTag);
	const TArray<TObjectPtr<UNetherCrownUIScreenBase>>& GetLayerContainer(const FGameplayTag& LayerTag) const;
	UNetherCrownUIScreenBase* GetTopPriorityScreen() const;
	void ApplyInputModeFromTopScreen();
	void ApplyGameOnlyInputMode() const;

	UPROPERTY(Transient)
	TSubclassOf<UNetherCrownPrimaryLayout> PrimaryLayoutClass{};

	UPROPERTY(Transient)
	TObjectPtr<UNetherCrownPrimaryLayout> PrimaryLayoutWidget{};

	UPROPERTY(Transient)
	TMap<FGameplayTag, FNetherCrownUIScreenDefinition> ScreenDefinitionMap{};

	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<UNetherCrownUIScreenBase>> ActiveScreenMap{};

	UPROPERTY(Transient)
	TArray<TObjectPtr<UNetherCrownUIScreenBase>> GameLayerScreenStack{};

	UPROPERTY(Transient)
	TArray<TObjectPtr<UNetherCrownUIScreenBase>> GameMenuLayerScreenStack{};

	UPROPERTY(Transient)
	TArray<TObjectPtr<UNetherCrownUIScreenBase>> MenuLayerScreenStack{};

	UPROPERTY(Transient)
	TArray<TObjectPtr<UNetherCrownUIScreenBase>> ModalLayerScreenStack{};
};
