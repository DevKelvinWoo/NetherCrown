// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetherCrown/Data/NetherCrownEnemySkillData.h"
#include "NetherCrownEnemyCrownPrisonWall.generated.h"

class UNiagaraComponent;
class UCameraShakeBase;
class UStaticMeshComponent;

class ANetherCrownCharacter;

USTRUCT()
struct FNetherCrownCrownPrisonInitData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "CrownPrisonData")
	float ExplosionRadius{ 700.f };

	UPROPERTY(EditDefaultsOnly, Category = "CrownPrisonData")
	float ExplosionTimeOffset{ 3.f };

	UPROPERTY(EditDefaultsOnly, Category = "CrownPrisonData")
	FNetherCrownEnemyCrownPrisonTagData CrownPrisonTagData{};
};

UCLASS()
class NETHERCROWN_API ANetherCrownEnemyCrownPrisonWall : public AActor
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnCrownPrisonExplosionHit, ANetherCrownCharacter*);

public:
	ANetherCrownEnemyCrownPrisonWall();

	void InitCrownPrisonWall(const FNetherCrownCrownPrisonInitData& InCrownPrisonInitData);
	void StartRiseUpOrDownWall(const float InWallHiddenZOffset, const float InWallRiseDuration, const bool bRiseUp);

	FOnCrownPrisonExplosionHit& GetOnCrownPrisonExplosionHit() { return OnCrownPrisonExplosionHit; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void SetWallCollisionEnabled(const ECollisionEnabled::Type CollisionEnabled) const;
	void HandleRiseWallFinished();

	void StartExplosionTimer();
	void ExplosionPrisonWallMagic();

	void StartRiseDownWallTimer();
	void RiseDownWall();

	void StartDestroyTimer();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ActiveMagicRangeNiagaraEffect();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ActiveWallRaiseUpDownNiagaraEffect(const bool bRaiseUp);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ActiveExplosionNiagaraEffect();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayCrownPrisonSoundByTag(const FGameplayTag& SoundTag);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayRiseWallCameraShake();

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

	UPROPERTY(EditDefaultsOnly, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> RiseWallCameraShakeClass{};

	UPROPERTY(EditDefaultsOnly, Category = "CameraShake")
	float RiseWallCameraShakeInnerRadius{ 0.f };

	UPROPERTY(EditDefaultsOnly, Category = "CameraShake")
	float RiseWallCameraShakeOuterRadius{ 1200.f };

	UPROPERTY(EditDefaultsOnly, Category = "CameraShake")
	float RiseWallCameraShakeFalloff{ 1.f };

	UPROPERTY(Transient)
	TArray<TObjectPtr<UStaticMeshComponent>> WallStaticMeshComponents{};

	FNetherCrownCrownPrisonInitData CrownPrisonInitData{};

	FVector HiddenLocation{};
	FVector RaisedLocation{};

	float WallRiseElapsedTime{};
	float WallRiseDuration{};

	UPROPERTY(Replicated)
	float WallHiddenZOffset{};

	bool bIsRising{ false };
	bool bIsRiseUp{ true };

	UPROPERTY(Replicated)
	FVector WorldCameraShakeLocation{};

	FTimerHandle ExplosionTimer{};
	FTimerHandle RiseDownWallTimer{};
	FTimerHandle DestroyTimer{};

	FOnCrownPrisonExplosionHit OnCrownPrisonExplosionHit;
};
