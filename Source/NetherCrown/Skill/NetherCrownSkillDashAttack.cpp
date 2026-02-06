// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownSkillDashAttack.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraSystem.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Util/NetherCrownCollisionChannels.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Character/AnimInstance/NetherCrownCharacterAnimInstance.h"
#include "NetherCrown/Components/NetherCrownControlGhostTrailComponent.h"

#define DEBUG_SPHERE 0

void UNetherCrownSkillDashAttack::PlaySkillCosmetics()
{
	Super::PlaySkillCosmetics();
}

void UNetherCrownSkillDashAttack::ExecuteSkillGameplay()
{
	Super::ExecuteSkillGameplay();

	StartDashAttackTimer();
}

void UNetherCrownSkillDashAttack::InitSkillObject()
{
	Super::InitSkillObject();

	if (!GhostTrailNiagaraSystemSoft.IsNull())
	{
		GhostTrailNiagaraSystem = GhostTrailNiagaraSystemSoft.LoadSynchronous();
	}
}

TArray<AActor*> UNetherCrownSkillDashAttack::DetectDashAttackTargets() const
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return {};
	}

	TArray<AActor*> OverlappedActors{};
	const FVector DetectSpherePos{ SkillOwnerCharacter->GetActorLocation() + SkillOwnerCharacter->GetActorForwardVector() * SkillDetectSphereOffset };
	const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes{ UEngineTypes::ConvertToObjectType(ECC_Enemy) };
	UKismetSystemLibrary::SphereOverlapActors(this, DetectSpherePos, SkillDetectSphereOffset, ObjectTypes, ANetherCrownEnemy::StaticClass(),
		TArray<AActor*>(), OverlappedActors);

	OverlappedActors.Sort([&SkillOwnerCharacter](const AActor& A, const AActor& B)
	{
		return A.GetDistanceTo(SkillOwnerCharacter) < B.GetDistanceTo(SkillOwnerCharacter);
	});

#if DEBUG_SPHERE
	UKismetSystemLibrary::DrawDebugSphere(this, DetectSpherePos, SkillDetectSphereOffset, 16, FColor::Red, 10.f);
#endif

	OverlappedActors.SetNum(MaxTargetNum);

	return OverlappedActors;
}

void UNetherCrownSkillDashAttack::DashAttackToTargets()
{
	if (CachedTargetActors.IsEmpty())
	{
		return;
	}

	const UWorld* World{ GetWorld() };
	check(World);

	AActor* CurrentTargetActor{};
	if (CachedTargetActors.IsValidIndex(CurrentTargetIndex))
	{
		CurrentTargetActor = CachedTargetActors[CurrentTargetIndex];
	}
	else
	{
		ClearDashAttackData();
	}

	++CurrentTargetIndex;

	if (!IsValid(CurrentTargetActor))
	{
		ClearDashAttackData();
		UE_LOG(LogTemp, Warning, TEXT("Current Target Actor is InValid! : %hs"), __FUNCTION__);
		return;
	}

	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		ClearDashAttackData();
		UE_LOG(LogTemp, Warning, TEXT("SkillOwnerCharacter is InValid! : %hs"), __FUNCTION__);
		return;
	}

	const FVector CurrentTargetLocation{ CurrentTargetActor->GetActorLocation() };
	const FVector SkillOwnerLocation{ SkillOwnerCharacter->GetActorLocation() };

	const FRotator RotationToTarget{ UKismetMathLibrary::FindLookAtRotation(SkillOwnerLocation, CurrentTargetLocation) };
	Multicast_SetOwnerCharacterRotToTarget(RotationToTarget); //@NOTE : Need Multicast RPC to rotate in dedicate server env

	const FVector DashOffsetByDirection{ SkillOwnerCharacter->GetActorForwardVector() * EndLocationOffset };
	const FVector DashEndLocation{ CurrentTargetLocation + DashOffsetByDirection };
	Multicast_DashOwnerCharacter(SkillOwnerLocation, DashEndLocation); //@NOTE : Server에서만 호출 시 네트워크 복제 지연으로 끊김 발생
}

void UNetherCrownSkillDashAttack::Multicast_DeactivateDashAttackGhostTrail_Implementation()
{
	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	UNetherCrownControlGhostTrailComponent* ControlGhostTrailComponent{ SkillOwnerCharacter->GetControlGhostTrailComponent() };
	if (!ensureAlways(ControlGhostTrailComponent))
	{
		return;
	}

	ControlGhostTrailComponent->ActivateGhostTrail(false);
}

void UNetherCrownSkillDashAttack::ClearDashAttackData()
{
	const UWorld* World{ GetWorld() };
	check(World);

	CachedTargetActors.Empty();
	World->GetTimerManager().ClearTimer(DashAttackTimerHandle);

	CurrentTargetIndex = 0;

	Multicast_DeactivateDashAttackGhostTrail();
	Multicast_SetCharacterCapsuleCollisionData(false);
}

void UNetherCrownSkillDashAttack::Multicast_SetCharacterCapsuleCollisionData_Implementation(const bool bStartDashAttack)
{
	//@NOTE : Server에서만 실행 시 Collision 변경 지연으로 살짝 부딪히는 느낌이 발생함
	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	UCapsuleComponent* SkillOwnerCapsuleComponent{ SkillOwnerCharacter->GetCapsuleComponent() };
	if (!ensureAlways(SkillOwnerCapsuleComponent))
	{
		return;
	}

	const ECollisionResponse CollisionEnabled{ bStartDashAttack ? ECollisionResponse::ECR_Overlap : ECollisionResponse::ECR_Block };
	SkillOwnerCapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, CollisionEnabled);
}

void UNetherCrownSkillDashAttack::Multicast_DashOwnerCharacter_Implementation(const FVector StartLoc, const FVector TargetVec)
{
	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	UNetherCrownControlGhostTrailComponent* ControlGhostTrailComponent{ SkillOwnerCharacter->GetControlGhostTrailComponent() };
	if (!ensureAlways(ControlGhostTrailComponent))
	{
		return;
	}

	// const USkeletalMeshComponent* SkeletalMeshComponent{ SkillOwnerCharacter->GetMesh() };
	// UNetherCrownCharacterAnimInstance* NetherCrownCharacterAnimInstance{};
	// NetherCrownCharacterAnimInstance = SkeletalMeshComponent ? Cast<UNetherCrownCharacterAnimInstance>(SkeletalMeshComponent->GetAnimInstance()) : nullptr;
	// if (!ensureAlways(IsValid(NetherCrownCharacterAnimInstance)))
	// {
	// 	return;
	// }
	//
	// UAnimMontage* SkillAnimMontage{ SkillAnimMontageSoft.LoadSynchronous() };
	// if (!ensureAlways(IsValid(SkillAnimMontage)))
	// {
	// 	return;
	// }
	//
	// NetherCrownCharacterAnimInstance->Montage_Play(SkillAnimMontage);

	ControlGhostTrailComponent->ActivateGhostTrail(true, GhostTrailNiagaraSystem);

	if (SkillOwnerCharacter->GetCharacterMovement())
	{
		//@NOTE : 힘을 가하려면 FRootMotionSource_ConstantForce로 처리, MoveToForce는 특정 지점으로 처리하는 방식
		TSharedPtr<FRootMotionSource_MoveToForce> MoveToForce = MakeShared<FRootMotionSource_MoveToForce>();

		MoveToForce->InstanceName = TEXT("DashAttack");
		MoveToForce->AccumulateMode = ERootMotionAccumulateMode::Override;
		MoveToForce->Priority = 5;
		MoveToForce->StartLocation = StartLoc;
		MoveToForce->TargetLocation = TargetVec;
		MoveToForce->Duration = DashDuration;
		MoveToForce->bRestrictSpeedToExpected = true;

		MoveToForce->FinishVelocityParams.Mode = ERootMotionFinishVelocityMode::SetVelocity;
		MoveToForce->FinishVelocityParams.SetVelocity = FVector::ZeroVector;
		SkillOwnerCharacter->GetCharacterMovement()->ApplyRootMotionSource(MoveToForce);
	}
}

void UNetherCrownSkillDashAttack::Multicast_SetOwnerCharacterRotToTarget_Implementation(const FRotator& InTargetRot)
{
	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	SkillOwnerCharacter->SetActorRotation(InTargetRot);
}

void UNetherCrownSkillDashAttack::StartDashAttackTimer()
{
	const UWorld* World{ GetWorld() };
	check(World);

	ClearDashAttackData();
	Multicast_SetCharacterCapsuleCollisionData(true);

	CachedTargetActors = DetectDashAttackTargets();
	if (CachedTargetActors.IsEmpty())
	{
		return;
	}

	World->GetTimerManager().SetTimer(DashAttackTimerHandle, this, &ThisClass::DashAttackToTargets, DashTimerRate, true, 0.f);
}
