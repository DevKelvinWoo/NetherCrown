// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrownInteractComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownInteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownInteractComponent();

	void SetTargetInteractActor(AActor* InTargetInteractActor);

	void InteractToTarget();

	void FinishInteract();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION(Client, Reliable)
	void Client_MoveCameraToInteractPosition();

	void RestoreCameraPosition();
	void RestoreHiddenScreensAfterInteraction() const;

	UFUNCTION(Server, Reliable)
	void Server_InteractToTarget();

	UFUNCTION(Client, Reliable)
	void Client_PlayInteractSound();

	UFUNCTION(Server, Reliable)
	void Server_FinishInteract();

	UPROPERTY(Transient, Replicated)
	TObjectPtr<AActor> TargetInteractActor{};

	UPROPERTY(Transient)
	TObjectPtr<AActor> PreviousViewTarget{};

	UPROPERTY(Transient)
	TObjectPtr<ACameraActor> InteractCameraActor{};
};
