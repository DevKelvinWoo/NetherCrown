// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrownDamageReceiverComponent.generated.h"


class UNetherCrownCharacterDamageReceiveDataAsset;
class ANetherCrownCharacter;

UENUM()
enum class ENetherCrownHitReactState : uint8
{
	None,
	HitReact
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownDamageReceiverComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownDamageReceiverComponent();

	float HandleIncomingDamage(float DamageAmount, FDamageEvent const& DamageEvent, AActor* DamageCauser);

	bool IsHitReacting() const { return HitReactState == ENetherCrownHitReactState::HitReact; }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void CacheOwnerCharacter();
	void CacheDamageReceiveDataAsset();
	void CacheHitReactAnimMontage();

	void SetHitReactStateAndTimer();

	UFUNCTION(Client, Unreliable)
	void Client_PlayHitCameraShake();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayHitImpactEffect();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayHitReactAnimation();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayHitImpactSound();

	float CalculateFinalDamage(float DamageAmount, FDamageEvent const& DamageEvent, const AActor* DamageCauser) const;
	void ApplyFinalDamage(float FinalDamage);

	int32 GetArmorStat(const bool bIsPhysicalDamage) const;

	bool IsDead() const;

	UPROPERTY(Transient)
	TObjectPtr<ANetherCrownCharacter> CachedOwnerCharacter{};

	UPROPERTY(Transient)
	TObjectPtr<UNetherCrownCharacterDamageReceiveDataAsset> CachedDamageReceiveDataAsset{};

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CachedHitReactAnimMontage{};

	UPROPERTY(EditDefaultsOnly, Category = "DamageReceiverData")
	TSoftObjectPtr<UNetherCrownCharacterDamageReceiveDataAsset> DamageReceiveDataAssetSoft{};

	UPROPERTY(Replicated)
	ENetherCrownHitReactState HitReactState{ ENetherCrownHitReactState::None };

	FTimerHandle HitReactTimerHandle;
};
