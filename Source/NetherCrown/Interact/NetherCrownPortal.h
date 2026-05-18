// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "NetherCrownPortal.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class NETHERCROWN_API ANetherCrownPortal : public AActor
{
	GENERATED_BODY()

public:
	ANetherCrownPortal();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnTestBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditDefaultsOnly, Category = "Tag")
	FGameplayTag LevelTag{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UBoxComponent> TestBoxComponent{};
};
