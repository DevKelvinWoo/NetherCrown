// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "NetherCrown/Data/NetherCrownEnemySkillData.h"
#include "NetherCrownEnemySkillObject.generated.h"

class UAnimMontage;
class UDamageType;

class ANetherCrownEnemy;
class ANetherCrownCharacter;

UCLASS()
class NETHERCROWN_API UNetherCrownEnemySkillObject : public UObject
{
	GENERATED_BODY()
	DECLARE_MULTICAST_DELEGATE(FOnEnemySkillFinished);

public:
	void SetSkillOwnerEnemy(ANetherCrownEnemy* SkillOwnerEnemy);

	const FGameplayTag& GetEnemySkillTag() const { return EnemySkillTag; }
	FOnEnemySkillFinished& GetOnEnemySkillFinished() { return OnEnemySkillFinished; }

	virtual void InitEnemySkillObject();
	virtual void PlayEnemySkillCosmetics();
	virtual void ExecuteEnemySkillGameplay();

	bool IsEnemySkillCoolDown() const { return bIsCoolDown; }

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

	void FinishEnemySkill();

	int32 CalculateEnemyMagicSkillDamage() const;
	void ApplyEnemyMagicSkillDamage(ANetherCrownCharacter* TargetCharacter) const;

private:
	void CacheSkillData();
	void ApplyEnemySkillDamage(ANetherCrownCharacter* TargetCharacter, int32 DamageAmount, TSubclassOf<UDamageType> DamageTypeClass) const;

	void StartSkillCoolDownTimer();
	void StopSkillCoolDownTimer();
	void StartEnemySkillDurationTimer();

	UPROPERTY(Replicated)
	bool bIsCoolDown{ false };

	FTimerHandle SkillCoolDownTimerHandle{};
	FTimerHandle TimerHandle_EnemySkillFinished{};
	FOnEnemySkillFinished OnEnemySkillFinished;
};
