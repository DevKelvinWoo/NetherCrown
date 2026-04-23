// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrownActionControlComponent.generated.h"


class UNetherCrownCrowdControlComponent;
class UNetherCrownDamageReceiverComponent;
class UNetherCrownEquipComponent;
class ANetherCrownCharacter;
class UNetherCrownSkillComponent;
class UNetherCrownBasicAttackComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownActionControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownActionControlComponent();

	bool CanMove() const;
	bool CanAttack() const;
	bool CanPlayHitReactionAnimation() const;

protected:
	virtual void BeginPlay() override;

private:
	void CacheInitData();

	TWeakObjectPtr<UNetherCrownEquipComponent> EquipComponentWeak{};
	TWeakObjectPtr<UNetherCrownBasicAttackComponent> BasicAttackComponentWeak{};
	TWeakObjectPtr<UNetherCrownSkillComponent> SkillComponentWeak{};
	TWeakObjectPtr<UNetherCrownDamageReceiverComponent> DamageReceiverComponentWeak{};
	TWeakObjectPtr<UNetherCrownCrowdControlComponent> CrowdControlComponentWeak{};

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownCharacter> CachedOwnerCharacter{};
};
