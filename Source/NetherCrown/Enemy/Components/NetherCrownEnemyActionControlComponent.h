// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrownEnemyActionControlComponent.generated.h"

class ANetherCrownEnemy;
class UNetherCrownEnemyDamageReceiverComponent;
class UNetherCrownEnemyBasicAttackComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownEnemyActionControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownEnemyActionControlComponent();

	bool CanAttack() const;
	bool CanChase() const;

protected:
	virtual void BeginPlay() override;

private:
	void CacheInitData();

	TWeakObjectPtr<UNetherCrownEnemyBasicAttackComponent> EnemyBasicAttackComponentWeak{};
	TWeakObjectPtr<UNetherCrownEnemyDamageReceiverComponent> EnemyDamageReceiverComponentWeak{};

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownEnemy> CachedOwnerEnemy{};
};
