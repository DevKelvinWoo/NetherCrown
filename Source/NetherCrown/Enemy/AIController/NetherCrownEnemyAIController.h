// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "NetherCrownEnemyAIController.generated.h"

class ANetherCrownCharacter;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UBehaviorTree;
class UBlackboardComponent;

UCLASS()
class NETHERCROWN_API ANetherCrownEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	ANetherCrownEnemyAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	void SetSightConfigAndPerceptionComponentValue();

	UPROPERTY(EditDefaultsOnly, Category = "Perception")
	float SightRadius{ 1200.0f };

	UPROPERTY(EditDefaultsOnly, Category = "Perception")
	float LoseSightRadius{ 1500.0f };

	UPROPERTY(EditDefaultsOnly, Category = "Perception")
	float PeripheralVisionAngleDegrees{ 70.0f };

	UPROPERTY(EditDefaultsOnly, Category = "Perception")
	float MaxSightAge{ 2.0f };

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetActorBlackboardKey{ TEXT("TargetActor") };

	UPROPERTY(EditDefaultsOnly, Category = "Perception")
	TObjectPtr<UAIPerceptionComponent> EnemyPerceptionComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig{};

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset{};

	UPROPERTY(Transient)
	TObjectPtr<UBlackboardComponent> BlackboardComponentCached{};
};
