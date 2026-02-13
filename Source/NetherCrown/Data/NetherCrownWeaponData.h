// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "NetherCrownWeaponData.generated.h"

UCLASS()
class NETHERCROWN_API UNetherCrownWeaponData : public UDataAsset
{
	GENERATED_BODY()

public:
	int32 GetWeaponAttackDamage() const { return WeaponAttackDamage; }
	int32 GetPhysicalPenetration() const { return PhysicalPenetration; }

private:
	UPROPERTY(EditAnywhere)
	FString WeaponName{};

	UPROPERTY(EditAnywhere)
	int32 WeaponAttackDamage{};

	UPROPERTY(EditAnywhere)
	int32 WeaponMagicPower{};

	UPROPERTY(EditAnywhere)
	int32 PhysicalPenetration{};

	UPROPERTY(EditAnywhere)
	int32 MagicPenetration{};

	UPROPERTY(EditAnywhere)
	int32 WeaponMovementSpeed{};

	UPROPERTY(EditAnywhere)
	int32 WeaponCoolDownReduction{};
};

USTRUCT(BlueprintType)
struct FNetherCrownWeaponDataTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	const FGameplayTag& GetWeaponTag() const { return WeaponTag; }
	TSoftObjectPtr<UNetherCrownWeaponData> GetWeaponData() const { return WeaponData; }

private:
	UPROPERTY(EditAnywhere)
	FGameplayTag WeaponTag{};

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UNetherCrownWeaponData> WeaponData{};
};
