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

private:
	void CalculateNextComboCount();

	void StartAttackBasic();

	void PlayAndJumpToComboMontageSection(const FName* SectionName) const;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> BasicAttackAnimMontageSoft{};

	UPROPERTY(EditDefaultsOnly)
	TMap<int32, FName> ComboMontageSectionMap{};

	int32 CurrentComboCount{ 1 };

	bool bCanQueueNextCombo{ false };
	bool bHasQueuedNextCombo{ false };

	bool bCanInputFirstAttack{ true };
};
