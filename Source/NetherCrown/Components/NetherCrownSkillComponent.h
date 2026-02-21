// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "NetherCrown/Skill/NetherCrownSkillObject.h"
#include "NetherCrownSkillComponent.generated.h"

class ANetherCrownCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownSkillComponent : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnStopOrStartSkill, const bool);

public:
	UNetherCrownSkillComponent();

	void ActivateSkill(const ENetherCrownSkillKeyEnum SkillKeyEnum);

	void SetActiveSkillSlowPlayRate(const bool bBeginSlow);

	FOnStopOrStartSkill& GetOnStopOrStartSkill() { return OnStopOrStartSkill; }

	bool CanActivateSkill() const;

	ENetherCrownSkillKeyEnum GetActiveSkillKeyEnum() const { return ActiveSkillKeyEnum; }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void ConstructSkillObjects();
	void CacheInitData();

	UFUNCTION(Server, Reliable)
	void Server_ActivateSkill(const ENetherCrownSkillKeyEnum SkillKeyEnum);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySkillCosmetics(UNetherCrownSkillObject* FoundSkillObject);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetActiveSkillKeyEnum(const ENetherCrownSkillKeyEnum SkillKeyEnum);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetActiveSkillSlowPlayRate(const bool bBeginSlow);

	UFUNCTION()
	void OnRep_ReplicatedSkillObjects();

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UNetherCrownSkillObject>> SkillObjectClasses{};

	UPROPERTY()
	TMap<ENetherCrownSkillKeyEnum, UNetherCrownSkillObject*> SkillObjects{};

	UPROPERTY(ReplicatedUsing=OnRep_ReplicatedSkillObjects)
	TArray<TObjectPtr<UNetherCrownSkillObject>> ReplicatedSkillObjects;

	UPROPERTY(Replicated)
	ENetherCrownSkillKeyEnum ActiveSkillKeyEnum{ ENetherCrownSkillKeyEnum::None };

	UPROPERTY()
	TObjectPtr<ANetherCrownCharacter> CachedCharacter{};

	FOnStopOrStartSkill OnStopOrStartSkill;
};
