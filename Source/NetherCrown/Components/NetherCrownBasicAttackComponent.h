// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrownBasicAttackComponent.generated.h"

class UAnimMontage;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownBasicAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void RequestBasicAttack();

	void EnableComboWindow();
	void DisableComboAndPlayQueuedComboWindow();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void CalculateNextComboCount();

	void StartAttackBasic();

	void PlayAndJumpToComboMontageSection(const FName* SectionName) const;

	UFUNCTION(Server, Reliable)
	void Server_PlayAndJumpToComboMontageSection();

	UFUNCTION(Server, Reliable)
	void Server_PlayNextComboSection();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAndJumpToComboMontageSection(const FName& SectionName);

	UFUNCTION(Server, Reliable)
	void Server_SetCanInputFirstAttack(const bool InbCanInputFirstAttack);

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> BasicAttackAnimMontageSoft{};

	UPROPERTY(EditDefaultsOnly)
	TMap<int32, FName> ComboMontageSectionMap{};

	int32 CurrentComboCount{ 1 };

	bool bCanQueueNextCombo{ false };
	bool bHasQueuedNextCombo{ false };

	UPROPERTY(Replicated)
	bool bCanInputFirstAttack{ true };
};
