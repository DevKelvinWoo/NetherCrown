// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "NetherCrownUITypes.h"
#include "NetherCrownUIScreenBase.generated.h"

class ANetherCrownCharacter;
class ANetherCrownPlayerController;
class ANetherCrownPlayerState;

UCLASS(Abstract, Blueprintable)
class NETHERCROWN_API UNetherCrownUIScreenBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	const FGameplayTag& GetScreenTag() const { return ScreenTag; }

	UFUNCTION(BlueprintCallable)
	const FGameplayTag& GetLayerTag() const { return LayerTag; }

	UFUNCTION(BlueprintCallable)
	ENetherCrownUIInputMode GetInputMode() const { return InputMode; }

	UFUNCTION(BlueprintCallable)
	bool ShouldShowMouseCursor() const { return bShowMouseCursor; }

	UFUNCTION(BlueprintCallable)
	ANetherCrownPlayerController* GetOwningNetherCrownPlayerController() const;

	UFUNCTION(BlueprintCallable)
	ANetherCrownCharacter* GetOwningNetherCrownCharacter() const;

	UFUNCTION(BlueprintCallable)
	ANetherCrownPlayerState* GetOwningNetherCrownPlayerState() const;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void HandleScreenShown();

	UFUNCTION(BlueprintImplementableEvent)
	void HandleScreenHidden();

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FGameplayTag ScreenTag{};

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FGameplayTag LayerTag{};

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	ENetherCrownUIInputMode InputMode{ ENetherCrownUIInputMode::GameAndUI };

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	bool bShowMouseCursor{ true };
};
