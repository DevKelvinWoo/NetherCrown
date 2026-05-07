// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetherCrownEnemyCrownPrisonWall.generated.h"

class UNiagaraComponent;
class UStaticMeshComponent;

UCLASS()
class NETHERCROWN_API ANetherCrownEnemyCrownPrisonWall : public AActor
{
	GENERATED_BODY()

public:
	ANetherCrownEnemyCrownPrisonWall();

	void StartRiseWall(const float InWallHiddenZOffset, const float InWallRiseDuration);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void SetWallCollisionEnabled(const ECollisionEnabled::Type CollisionEnabled) const;
	void HandleRiseWallFinished();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ActiveMagicRangeNiagaraEffect();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ActiveWallRaiseUpDownNiagaraEffect(const bool bRaiseUp);

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UStaticMeshComponent> WallStaticMeshComponent1{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UStaticMeshComponent> WallStaticMeshComponent2{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UStaticMeshComponent> WallStaticMeshComponent3{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UStaticMeshComponent> WallStaticMeshComponent4{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UStaticMeshComponent> WallStaticMeshComponent5{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UStaticMeshComponent> WallStaticMeshComponent6{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNiagaraComponent> MagicRangeNiagaraComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNiagaraComponent> ExplosionNiagaraComponent{};

	UPROPERTY(Transient)
	TArray<TObjectPtr<UStaticMeshComponent>> WallStaticMeshComponents{};

	FVector HiddenLocation{};
	FVector RaisedLocation{};

	float WallRiseElapsedTime{};
	float WallRiseDuration{};

	UPROPERTY(Replicated)
	float WallHiddenZOffset{};

	bool bIsRising{ false };
};
