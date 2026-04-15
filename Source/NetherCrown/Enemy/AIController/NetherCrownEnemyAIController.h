// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "NetherCrown/Data/NetherCrownEnemyAITuningData.h"
#include "NetherCrownEnemyAIController.generated.h"

class ANetherCrownCharacter;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UBlackboardComponent;

UCLASS()
class NETHERCROWN_API ANetherCrownEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	ANetherCrownEnemyAIController();

	float GetPatrolRadius() const { return EnemyAITuningData.PatrolRadius; }

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	void LoadEnemyAITuningData();
	void SetSightConfigAndPerceptionComponentValue();

	UPROPERTY(Transient)
	FNetherCrownEnemyAITuningData EnemyAITuningData{};

	UPROPERTY(EditDefaultsOnly, Category = "EnemyAITuningDataAsset")
	TSoftObjectPtr<UNetherCrownEnemyAITuningDataAsset> EnemyAITuningDataAssetSoft{};

	UPROPERTY(EditDefaultsOnly, Category = "Perception")
	TObjectPtr<UAIPerceptionComponent> EnemyPerceptionComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig{};

	UPROPERTY(Transient)
	TObjectPtr<UBlackboardComponent> BlackboardComponentCached{};
};
