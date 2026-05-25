// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "NetherCrownSequenceInteractActor.generated.h"

class ANetherCrownCharacter;
class UBoxComponent;

UCLASS()
class NETHERCROWN_API ANetherCrownSequenceInteractActor : public AActor
{
	GENERATED_BODY()

public:
	ANetherCrownSequenceInteractActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void HandleOnSequenceFinished(){};

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	TWeakObjectPtr<ANetherCrownCharacter> InteractCharacterWeak{};

private:
	UFUNCTION()
	void HandleOnInteractBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySequence();

	UPROPERTY(EditDefaultsOnly, Category = "Tag")
	FGameplayTag LevelSequenceTag{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UBoxComponent> InteractBoxComponent{};
};
