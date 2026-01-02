// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrownCrowdControlComponent.generated.h"

class UAnimMontage;

UENUM()
enum class ENetherCrownCrowdControlType : uint8
{
	NONE,
	KNOCK_BACK,
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownCrowdControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownCrowdControlComponent();

	ENetherCrownCrowdControlType GetCrowdControlType() const { return CrowdControlType; }
	void ApplyCrowdControl(const ENetherCrownCrowdControlType InCrowdControlType, float DurationTime);

	void KnockBack(const FVector& KnockBackVector) const;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayCrowdControlAnim(const ENetherCrownCrowdControlType InCrowdControlType);

	void ClearCrowdControl();

	void PlayCrowdControlAnim(const ENetherCrownCrowdControlType InCrowdControlType);

	FTimerHandle CrowdControlTimerHandle{};

	UPROPERTY(EditDefaultsOnly)
	TMap<ENetherCrownCrowdControlType, TSoftObjectPtr<UAnimMontage>> CrowdControlAnimMap{};

	UPROPERTY(Replicated)
	ENetherCrownCrowdControlType CrowdControlType{ ENetherCrownCrowdControlType::NONE };
};
