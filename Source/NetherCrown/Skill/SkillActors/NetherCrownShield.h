// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "NetherCrownShield.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UCurveFloat;

UCLASS(Blueprintable)
class NETHERCROWN_API ANetherCrownShield : public AActor
{
	GENERATED_BODY()

public:
	ANetherCrownShield();

	void DestroyShield();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	void CreateShieldDynamicMaterialInstance();
	void BindTimelineFunctions();

	void StartSetBeginShieldMaterialTimeline();
	void StartSetEndShieldMaterialTimeline();

	UFUNCTION()
	void SetBeginShieldMaterialByFloatTimeline(float FloatCurveValue);

	UFUNCTION()
	void SetEndShieldMaterialByFloatTimeline(float FloatCurveValue);

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UStaticMeshComponent> ShieldMeshComponent{};

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ShieldDynamicMaterialInstance{};

	UPROPERTY(EditDefaultsOnly, Category = "CurveData")
	TSoftObjectPtr<UCurveFloat> ShieldMaterialStartCurveFloatSoft{};

	UPROPERTY(EditDefaultsOnly, Category = "CurveData")
	TSoftObjectPtr<UCurveFloat> ShieldMaterialEndCurveFloatSoft{};

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> CachedStartCurveFloat{};

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> CachedEndCurveFloat{};

	UPROPERTY(EditDefaultsOnly, Category = "MaterialParam")
	FName ShieldMaterialScalarParameterName{ TEXT("ShieldAlpha") };

	FTimeline BeginShieldMaterialFloatTimeline{};
	FTimeline EndShieldMaterialFloatTimeline{};
};
