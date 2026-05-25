// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetherCrownDamageNumberActor.generated.h"

class USceneComponent;
class UWidgetComponent;

UCLASS()
class NETHERCROWN_API ANetherCrownDamageNumberActor : public AActor
{
	GENERATED_BODY()

public:
	ANetherCrownDamageNumberActor();

	void InitDamageNumber(const int32 DamageAmount, const bool bIsCriticalDamage);

private:
	void HandleDamageNumberAnimationFinished();

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<USceneComponent> SceneRootComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UWidgetComponent> DamageNumberWidgetComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "DamageNumber")
	float FallbackLifeSpan{ 1.5f };
};
