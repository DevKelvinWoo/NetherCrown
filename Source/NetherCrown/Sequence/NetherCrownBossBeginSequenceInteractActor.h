// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownSequenceInteractActor.h"
#include "NetherCrownBossBeginSequenceInteractActor.generated.h"

class ANetherCrownBossEnemy;

UCLASS()
class NETHERCROWN_API ANetherCrownBossBeginSequenceInteractActor : public ANetherCrownSequenceInteractActor
{
	GENERATED_BODY()

public:
	ANetherCrownBossBeginSequenceInteractActor();

protected:
	virtual void BeginPlay() override;

	virtual void HandleOnSequenceFinished() override;

private:
	UPROPERTY(EditInstanceOnly, Category = "Boss")
	TObjectPtr<ANetherCrownBossEnemy> BossEnemy{};

	UPROPERTY(EditInstanceOnly, Category = "Boss")
	TObjectPtr<AActor> SequenceDummyActor{};
};
