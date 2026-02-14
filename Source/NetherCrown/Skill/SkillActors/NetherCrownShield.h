// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

private:
	void CreateShieldDynamicMaterialInstance();

	void StartShieldMaterialCurveTimer(const bool bStartShield);
	void ApplyBeginShieldMaterialParameterCurveFloat();
	void ApplyEndShieldMaterialParameterCurveFloat();

	void SetBeginShieldDynamicMaterialScalarParam() const;
	void SetEndShieldDynamicMaterialScalarParam();

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> ShieldMeshComponent{};

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ShieldDynamicMaterialInstance{};

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UCurveFloat> ShieldMaterialStartCurveFloatSoft{};

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UCurveFloat> ShieldMaterialEndCurveFloatSoft{};

	UPROPERTY()
	TObjectPtr<UCurveFloat> CachedStartCurveFloat{};

	UPROPERTY()
	TObjectPtr<UCurveFloat> CachedEndCurveFloat{};

	FTimerHandle ShieldMaterialCurveTimerHandle{};
	float ShieldMaterialCurveElapsedTime{ 0.f };

	UPROPERTY(EditDefaultsOnly)
	FName ShieldMaterialScalarParameterName{ TEXT("ShieldAlpha") };
};
