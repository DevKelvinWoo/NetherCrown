// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "NetherCrownEnemySkillComponent.generated.h"

class ANetherCrownEnemy;
class UNetherCrownEnemySkillObject;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownEnemySkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownEnemySkillComponent();

	void ActivateEnemySkill(const FGameplayTag& SkillTag);
	UNetherCrownEnemySkillObject* GetEnemySkillObject(const FGameplayTag& SkillTag) const;

protected:
	virtual void BeginPlay() override;

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void CacheInitData();
	void ConstructEnemySkillObjects();

	UFUNCTION()
	void OnRep_ReplicatedEnemySkillObjects();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayEnemySkillCosmetics(UNetherCrownEnemySkillObject* FoundEnemySkillObject);

	UPROPERTY(EditDefaultsOnly, Category = "SkillObject")
	TArray<TSubclassOf<UNetherCrownEnemySkillObject>> EnemySkillObjectClasses{};

	UPROPERTY()
	TMap<FGameplayTag, UNetherCrownEnemySkillObject*> EnemySkillObjectMap{};

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownEnemy> CachedOwnerEnemy{};

	UPROPERTY(ReplicatedUsing=OnRep_ReplicatedEnemySkillObjects)
	TArray<TObjectPtr<UNetherCrownEnemySkillObject>> ReplicatedEnemySkillObjects;
};
