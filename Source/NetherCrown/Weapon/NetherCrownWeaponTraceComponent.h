// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrown/Data/NetherCrownWeaponData.h"
#include "NetherCrownWeaponTraceComponent.generated.h"

class USkeletalMeshComponent;

class ANetherCrownEnemy;

UENUM()
enum class ENetherCrownTraceMode : uint8
{
	Swing,
	Thrust
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownWeaponTraceComponent : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHitEnemy, ANetherCrownEnemy*, const FVector&);

public:
	UNetherCrownWeaponTraceComponent();

	void SetWeaponHitTraceEnable(const bool bEnableWeaponHitTrace) { bIsTraceEnabled = bEnableWeaponHitTrace; }
	void InitWeaponTraceComponentSettings(const FVector& InLastEndLocation);

	void SetTraceMode(const ENetherCrownTraceMode InTraceMode) { TraceMode = InTraceMode; }

	FOnHitEnemy& GetOnHitEnemy() { return OnHitEnemy; }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void CacheWeaponTraceData();
	void DetectWeaponHit();
	void DetectWeaponSwingHit();
	void DetectWeaponThrustHit();

	UFUNCTION(Server, Reliable)
	void Server_ReportHit(ANetherCrownEnemy* HitEnemy, const FVector& HitLocation);

	UPROPERTY(Replicated)
	bool bIsTraceEnabled{ false };

	UPROPERTY(Transient)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh{};

	UPROPERTY(Replicated)
	FVector LastEndLocation{};

	UPROPERTY(Transient)
	FNetherCrownWeaponTraceData WeaponTraceData{};

	UPROPERTY(Replicated)
	TArray<TObjectPtr<ANetherCrownEnemy>> HitIgnoreEnemies{}; //TSet은 Replicate X

	UPROPERTY(Transient, Replicated)
	ENetherCrownTraceMode TraceMode{ ENetherCrownTraceMode::Swing };

	FOnHitEnemy OnHitEnemy;
};
