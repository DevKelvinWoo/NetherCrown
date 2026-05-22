// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NetherCrownInteract.h"
#include "GameFramework/Actor.h"
#include "NetherCrownInteractActor.generated.h"

class UWidgetComponent;
class USphereComponent;
class UTexture2D;

class ANetherCrownCharacter;

UCLASS()
class NETHERCROWN_API ANetherCrownInteractActor : public AActor, public INetherCrownInteract
{
	GENERATED_BODY()

public:
	ANetherCrownInteractActor();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Interact() override;

	void SetTargetInteractActor(const ANetherCrownCharacter* InteractCharacter, bool bTargetValid);
	void SetInteractWidgetVisibility(const ANetherCrownCharacter* InteractTarget, bool bVisible) const;

	const FGameplayTag& GetInteractActorTag() const { return InteractActorTag; }

	UPROPERTY(Replicated)
	TWeakObjectPtr<ANetherCrownCharacter> InteractTargetCharacterWeak{};

private:
	void CacheInteractWidgetTexture();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetInteractWidgetVisibility(const ANetherCrownCharacter* InteractTarget, bool bVisible) const;

	UFUNCTION()
	void HandleOnDetectSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleOnDetectSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<USphereComponent> InteractDetectSphereComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UWidgetComponent> InteractWidgetComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "InteractWidget")
	TSoftObjectPtr<UTexture2D> InteractWidgetTextureSoft{};

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> CachedInteractWidgetTexture{};

	UPROPERTY(EditDefaultsOnly, Category = "Tag")
	FGameplayTag InteractActorTag{};
};
