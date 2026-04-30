// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "NetherCrownEnemyMagicProjectile.generated.h"

class UNiagaraComponent;
class UProjectileMovementComponent;
class USphereComponent;

USTRUCT()
struct FNetherCrownEnemyMagicProjectileInitData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FVector Direction{};

	UPROPERTY()
	float ProjectileSpeed{ 0.f };

	UPROPERTY()
	FGameplayTag DestroyNiagaraEffectTag{};

	UPROPERTY()
	FGameplayTag DestroySoundTag{};
};

UCLASS()
class NETHERCROWN_API ANetherCrownEnemyMagicProjectile : public AActor
{
	GENERATED_BODY()

public:
	ANetherCrownEnemyMagicProjectile();

	void InitProjectile(const FNetherCrownEnemyMagicProjectileInitData& InitData);

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void HandleOnHitSphereBeginOverlap(UPrimitiveComponent* OnComponentBeginOverlap, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnDestroyProjectileEffect(const FTransform& DestroyTransform);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayDestroyProjectileSound();

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<USphereComponent> HitSphereComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNiagaraComponent> ProjectileNiagaraComponent{};

	UPROPERTY(Replicated)
	FGameplayTag DestroyNiagaraEffectTag{};

	UPROPERTY(Replicated)
	FGameplayTag DestroySoundTag{};
};
