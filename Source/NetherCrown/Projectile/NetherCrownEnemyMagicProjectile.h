// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "NetherCrownEnemyMagicProjectile.generated.h"

class UNiagaraComponent;
class UProjectileMovementComponent;
class USphereComponent;

class ANetherCrownCharacter;
class ANetherCrownEnemy;

USTRUCT()
struct FNetherCrownEnemyMagicProjectileInitData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FVector Direction{};

	UPROPERTY()
	float ProjectileSpeed{};

	UPROPERTY()
	FGameplayTag DestroyNiagaraEffectTag{};

	UPROPERTY()
	FGameplayTag DestroySoundTag{};

	UPROPERTY()
	FGameplayTag HitImpactEffectTag{};

	UPROPERTY()
	FGameplayTag HitImpactSoundTag{};

	UPROPERTY()
	TWeakObjectPtr<ANetherCrownEnemy> OwnerEnemyWeak{};

	UPROPERTY()
	TWeakObjectPtr<ANetherCrownCharacter> ReflectedOwnerCharacterWeak{};

	UPROPERTY()
	int32 ProjectileDamage{};

	UPROPERTY()
	bool bReflected{ false };
};

UCLASS()
class NETHERCROWN_API ANetherCrownEnemyMagicProjectile : public AActor
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMagicProjectileHit, ANetherCrownCharacter*, ANetherCrownEnemyMagicProjectile*);

public:
	ANetherCrownEnemyMagicProjectile();

	void InitProjectile(const FNetherCrownEnemyMagicProjectileInitData& InitData);

	FOnMagicProjectileHit& GetOnMagicProjectileHit() { return OnMagicProjectileHit; }

	ANetherCrownEnemy* GetOwnerEnemy() const { return OwnerEnemy; }
	float GetProjectileSpeed() const;
	int32 GetProjectileDamage() const { return ProjectileDamage; }
	const FGameplayTag& GetDestroyNiagaraEffectTag() const { return DestroyNiagaraEffectTag; }
	const FGameplayTag& GetDestroySoundTag() const { return DestroySoundTag; }
	const FGameplayTag& GetHitImpactEffectTag() const { return HitImpactEffectTag; }
	const FGameplayTag& GetHitImpactSoundTag() const { return HitImpactSoundTag; }

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

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnHitImpactEffect(const FTransform& HitImpactTransform);

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

	UPROPERTY(Replicated)
	TObjectPtr<ANetherCrownEnemy> OwnerEnemy{};

	UPROPERTY(Replicated)
	TObjectPtr<ANetherCrownCharacter> ReflectedOwnerCharacter{};

	UPROPERTY(Replicated)
	FGameplayTag HitImpactEffectTag{};

	UPROPERTY(Replicated)
	FGameplayTag HitImpactSoundTag{};

	UPROPERTY(Replicated)
	int32 ProjectileDamage{};

	UPROPERTY(Replicated)
	bool bReflected{ false };

	FOnMagicProjectileHit OnMagicProjectileHit;
};
