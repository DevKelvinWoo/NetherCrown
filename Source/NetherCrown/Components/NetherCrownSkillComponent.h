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

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlaySkillCosmetics(UNetherCrownSkillObject* FoundSkillObject);

	UFUNCTION(Client, Unreliable)
	void Client_SetActiveSkillKeyEnum(const ENetherCrownSkillKeyEnum SkillKeyEnum);

	UFUNCTION()
	void OnRep_ReplicatedSkillObjects();

	UPROPERTY(EditDefaultsOnly, Category = "SkillObject")
	TArray<TSubclassOf<UNetherCrownSkillObject>> SkillObjectClasses{};

	UPROPERTY()
	TMap<ENetherCrownSkillKeyEnum, UNetherCrownSkillObject*> SkillObjectMap{};

	UPROPERTY(ReplicatedUsing=OnRep_ReplicatedSkillObjects)
	TArray<TObjectPtr<UNetherCrownSkillObject>> ReplicatedSkillObjects;

	UPROPERTY(Replicated)
	ENetherCrownSkillKeyEnum ActiveSkillKeyEnum{ ENetherCrownSkillKeyEnum::None };

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownCharacter> CachedCharacter{};

	FOnStopOrStartSkill OnStopOrStartSkill;
};
