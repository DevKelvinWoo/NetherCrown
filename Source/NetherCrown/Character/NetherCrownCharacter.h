// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "NetherCrown/Interface/NetherCrownCrowdControlInterface.h"
#include "NetherCrownCharacter.generated.h"

class ANetherCrownEnemy;
class UNetherCrownDamageReceiverComponent;
class UNiagaraComponent;
class USpringArmComponent;
class UCameraComponent;
class UPostProcessComponent;

class UNetherCrownControlPPComponent;
class UNetherCrownCrowdControlComponent;
class UNetherCrownControlGhostTrailComponent;
class UNetherCrownBasicAttackComponent;
class UNetherCrownEquipComponent;
class UNetherCrownSkillComponent;

enum class ENetherCrownSkillKeyEnum : uint8;

USTRUCT()
struct FNetherCrownCharacterTagData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag JumpStartSoundTag{};

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag HardLandingSoundTag{};
};

UCLASS()
class NETHERCROWN_API ANetherCrownCharacter : public ACharacter, public INetherCrownCrowdControlInterface
{
	GENERATED_BODY()
	DECLARE_MULTICAST_DELEGATE(FOnRepPlayerState);

public:
	ANetherCrownCharacter();

	void MoveCharacter(const FInputActionValue& Value);
	void LookAtCharacter(const FInputActionValue& Value);
	void JumpCharacter(const FInputActionValue& Value);
	void HandleOnMoveActionCompleted();
	void RequestBasicAttack(const FInputActionValue& Value);
	void EquipCharacter(const FInputActionValue& Value);
	void ChangeWeapon(const FInputActionValue& Value);

	void ExecuteSkillByKey(const FInputActionValue& Value, ENetherCrownSkillKeyEnum SkillKey) const;
	void ActiveQSkill(const FInputActionValue& Value) const;
	void ActiveESkill(const FInputActionValue& Value) const;
	void ActiveRSkill(const FInputActionValue& Value) const;
	void ActiveShiftSkill(const FInputActionValue& Value) const;

	void SetMainSpringArmZOffset(const float InSpringArmZOffset);
	void SetMainSpringArmLength(const float InSpringArmLength);
	float GetMainSpringArmLength() const;

	UFUNCTION(BlueprintCallable)
	bool IsPressedMoveKey() const { return bPressedMoveKey;}

	UFUNCTION(BlueprintCallable)
	bool IsHardLanding() const { return bIsHardLanding; }

	UFUNCTION(BlueprintCallable)
	bool IsEquippedWeapon() const;

	UFUNCTION(Server, Reliable)
	void Server_ReportHitByEnemy(ANetherCrownEnemy* HitCauserEnemy);

	UNetherCrownBasicAttackComponent* GetBasicAttackComponent() const { return NetherCrownBasicAttackComponent; }
	UNetherCrownEquipComponent* GetEquipComponent() const { return NetherCrownEquipComponent; }
	UNetherCrownSkillComponent* GetSkillComponent() const { return NetherCrownSkillComponent; }
	UNetherCrownControlPPComponent* GetControlPPComponent() const { return NetherCrownControlPPComponent; }
	UNetherCrownControlGhostTrailComponent* GetControlGhostTrailComponent() const { return NetherCrownControlGhostTrailComponent; }
	FOnRepPlayerState& GetOnRepPlayerState() { return OnRepPlayerState; }

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Landed(const FHitResult& Hit) override;
	virtual void OnJumped_Implementation() override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	virtual UNetherCrownStatusEffectControlComponent* GetStatusEffectControlComponent() const override;

	virtual void OnRep_PlayerState() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

private:
	void DestroyVisualOnlyComponentsOnDS();

	UFUNCTION(Server, Reliable)
	void Server_SetPressedMoveKey(const bool InbPressedMoveKey);

	void SetUseControllerSettings();
	void SetMainSpringArmComponentSettings();
	void SetMainCameraComponentSettings();
	void SetCharacterDefaultMovementSettings();

	void SetIsHardLanding();
	void ResetHardLandingState();
	void DisableMovementAndSetResetTimerWhenHardLanding();

	UFUNCTION()
	void OnRep_IsHardLanding();

	void SetCharacterMovementControl(const bool bEnableMovement);

	UPROPERTY(Replicated)
	bool bPressedMoveKey{ false };

	UPROPERTY(ReplicatedUsing = OnRep_IsHardLanding)
	bool bIsHardLanding{ false };

	UPROPERTY(Replicated)
	FVector JumpStartLocation{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UCameraComponent> MainCameraComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<USpringArmComponent> MainSpringArmComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNetherCrownBasicAttackComponent> NetherCrownBasicAttackComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNetherCrownEquipComponent> NetherCrownEquipComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNetherCrownSkillComponent> NetherCrownSkillComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNetherCrownCrowdControlComponent> NetherCrownCrowdControlComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UPostProcessComponent> NetherCrownPostProcessComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNetherCrownControlPPComponent> NetherCrownControlPPComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNiagaraComponent> NetherCrownGhostTrailNiagaraComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNetherCrownControlGhostTrailComponent> NetherCrownControlGhostTrailComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UNetherCrownDamageReceiverComponent> NetherCrownDamageReceiverComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "TagData")
	FNetherCrownCharacterTagData CharacterTagData{};

	FTimerHandle TimerHandle_ResetHardLanding;
	FOnRepPlayerState OnRepPlayerState;
};
