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
class UNetherCrownInteractActorDataAsset;

UCLASS()
class NETHERCROWN_API ANetherCrownInteractActor : public AActor, public INetherCrownInteract
{
	GENERATED_BODY()

public:
	ANetherCrownInteractActor();

	FVector GetInteractCameraPos() const;
	FRotator GetInteractCameraRot() const;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Interact() override;
	virtual void FinishInteract(ANetherCrownCharacter* InteractCharacter) override;

	void SetTargetInteractActor(const ANetherCrownCharacter* InteractCharacter, const bool bTargetValid);
	void SetInteractWidgetVisibility(const ANetherCrownCharacter* InteractTarget, const bool bVisible) const;

	void ToggleInteractBoxCollision(const bool bOn);

	const UNetherCrownInteractActorDataAsset* GetCachedInteractDataAsset() const { return CachedInteractDataAsset; }
	const FGameplayTag& GetInteractActorTag() const;

	UPROPERTY(Replicated)
	TWeakObjectPtr<ANetherCrownCharacter> InteractTargetCharacterWeak{};

private:
	void CacheInteractDataAsset();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowInteractActorDialogueWidget();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetInteractWidgetVisibility(const ANetherCrownCharacter* InteractTarget, const bool bVisible) const;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetInteractDetectSphereCollision(const bool bOn);

	UFUNCTION()
	void HandleOnDetectSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, const int32 OtherBodyIndex, const bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleOnDetectSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, const int32 OtherBodyIndex);

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<USphereComponent> InteractDetectSphereComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UWidgetComponent> InteractWidgetComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<USphereComponent> InteractCameraPosSphereComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TSoftObjectPtr<UNetherCrownInteractActorDataAsset> InteractDataAssetSoft{};

	UPROPERTY(Transient)
	TObjectPtr<UNetherCrownInteractActorDataAsset> CachedInteractDataAsset{};

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> CachedInteractWidgetTexture{};
};
