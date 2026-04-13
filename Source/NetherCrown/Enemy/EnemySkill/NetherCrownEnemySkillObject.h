// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "NetherCrownEnemySkillObject.generated.h"

class ANetherCrownEnemy;

USTRUCT()
struct FNetherCrownEnemySkillData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "EnemySkillAnim")
	TSoftObjectPtr<UAnimMontage> SkillAnimMontageSoft{};
};

UCLASS()
class NETHERCROWN_API UNetherCrownEnemySkillObject : public UObject
{
	GENERATED_BODY()

public:
	void SetSkillOwnerEnemy(ANetherCrownEnemy* SkillOwnerEnemy);

	const FGameplayTag& GetEnemySkillTag() const { return EnemySkillTag; }

	virtual void InitEnemySkillObject();
	virtual void PlayEnemySkillCosmetics();
	virtual void ExecuteEnemySkillGameplay();

protected:
	//@NOTE : To Replicate UObject
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	//@NOTE : To call RPC function in UObject
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack) override;

	UPROPERTY(Replicated)
	FNetherCrownEnemySkillData EnemySkillData{};

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CachedEnemySkillAnimMontage{};

	UPROPERTY(Replicated)
	TWeakObjectPtr<ANetherCrownEnemy> SkillOwnerEnemyWeak{};

	UPROPERTY(EditDefaultsOnly, Replicated, Category = "SkillTag")
	FGameplayTag EnemySkillTag{};

private:
	void CacheSkillData();
};
