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

	void ActiveSkill(const ENetherCrownSkillEnum SkillEnum);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UNetherCrownSkillObject>> SkillObjectClasses{};

	UPROPERTY()
	TMap<ENetherCrownSkillEnum, UNetherCrownSkillObject*> SkillObjects{};
};
