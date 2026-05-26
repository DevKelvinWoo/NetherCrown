// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrownParryComponent.generated.h"

class UNetherCrownParryDataAsset;
class ANetherCrownCharacter;
class ANetherCrownEnemyMagicProjectile;
class UNetherCrownPlayerStatComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownParryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownParryComponent();

	void ActiveParry();
	void DeactivateParry();
	bool IsParrying() const;

	void Parry(AActor* DamageCauser, const TSubclassOf<UDamageType> DamageTypeClass);

protected:
	virtual void BeginPlay() override;

private:
	void CacheCharacter();

	void CacheParryData();

	void ReflectEnemyMagicProjectile(ANetherCrownEnemyMagicProjectile* EnemyMagicProjectile) const;
	FVector GetMagicProjectileReflectDirection(const ANetherCrownEnemyMagicProjectile* EnemyMagicProjectile) const;

	void SetLocalParryState(bool bActive);
	void SetParryState(bool bActive);
	UNetherCrownPlayerStatComponent* GetPlayerStatComponent() const;

	UFUNCTION(Server, Reliable)
	void Server_ActiveParry();

	UFUNCTION(Server, Reliable)
	void Server_DeactivateParry();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayParrySoundAndEffect();

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownCharacter> CachedCharacter{};

	UPROPERTY(Transient)
	TObjectPtr<UNetherCrownParryDataAsset> CachedParryDataAsset{};

	UPROPERTY(EditDefaultsOnly, Category = "ParryData")
	TSoftObjectPtr<UNetherCrownParryDataAsset> ParryDataAssetSoft{};

	bool bLocalParrying{ false };
};
