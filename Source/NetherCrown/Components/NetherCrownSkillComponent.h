// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrown/Skill/NetherCrownSkillObject.h"
#include "NetherCrownSkillComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownSkillComponent();

	void ActiveSkill(const ENetherCrownSkillKeyEnum SkillKeyEnum);

	void SetActiveSkillSlowPlayRate(const bool bBeginSlow);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void ConstructSkillObjects();

	UFUNCTION(Server, Reliable)
	void Server_ActiveSkill(const ENetherCrownSkillKeyEnum SkillKeyEnum);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySkillCosmetics(UNetherCrownSkillObject* FoundSkillObject);

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
};
