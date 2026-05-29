// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetherCrownInteract.h"
#include "NetherCrownInteractActor.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "NetherCrownBossDungeonDoor.generated.h"

class UBoxComponent;
class USphereComponent;

UCLASS()
class NETHERCROWN_API ANetherCrownBossDungeonDoor : public ANetherCrownInteractActor
{
	GENERATED_BODY()

public:
	ANetherCrownBossDungeonDoor();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(const float DeltaTime) override;

	virtual void FinishInteract(ANetherCrownCharacter* InteractCharacter) override;

private:
	void CacheBossDungeonDoorData();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayOpenDoor();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayOpenDoorCameraShake();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayOpenDoorSound();

	UFUNCTION()
	void OpenLeftDoor(float CurveFloat);

	UFUNCTION()
	void OpenRightDoor(float CurveFloat);

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UStaticMeshComponent> LeftDoorMeshComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UStaticMeshComponent> RightDoorMeshComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<USphereComponent> CameraShakePointSphereComponent{};

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> CachedLeftDoorOpenCurve{};

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> CachedRightDoorOpenCurve{};

	FTimeline LeftDoorOpenTimeline{};
	FTimeline RightDoorOpenTimeline{};
};
