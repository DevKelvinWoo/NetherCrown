// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrownWeaponTraceComponent.generated.h"

class USkeletalMeshComponent;

class ANetherCrownEnemy;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownWeaponTraceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownWeaponTraceComponent();

	void SetWeaponHitTraceEnable(const bool bEnableWeaponHitTrace) { bIsTraceEnabled = bEnableWeaponHitTrace; }
	void InitWeaponTraceComponentSettings(const FVector& InLastEndLocation);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void DetectWeaponHit();

	UPROPERTY(Replicated)
	bool bIsTraceEnabled{ false };

	UPROPERTY(EditAnywhere)
	float TraceRadius = 10.0f;

	UPROPERTY(Transient)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh{};

	UPROPERTY(Replicated)
	FVector LastEndLocation{};

	UPROPERTY()
	FName WeaponTraceSocketName{};

	UPROPERTY(Replicated)
	TArray<TObjectPtr<ANetherCrownEnemy>> HitIgnoreEnemies{}; //TSet은 Replicate X
};
