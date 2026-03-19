// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownSkillDashAttack.h"

#include "NetherCrown/NetherCrown.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraSystem.h"
#include "Camera/CameraActor.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Util/NetherCrownCollisionChannels.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Character/NetherCrownPlayerController.h"
#include "NetherCrown/Character/AnimInstance/NetherCrownCharacterAnimInstance.h"
#include "NetherCrown/Components/NetherCrownControlGhostTrailComponent.h"
#include "NetherCrown/Components/NetherCrownControlPPComponent.h"
#include "NetherCrown/Components/NetherCrownCrowdControlComponent.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"

#define DEBUG_SPHERE 0

UNetherCrownSkillDashAttack::UNetherCrownSkillDashAttack()
{
	SetSkillTag(NetherCrownTags::Skill_DashAttack);
}

void UNetherCrownSkillDashAttack::PlaySkillCosmetics()
{
	Super::PlaySkillCosmetics();

	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	if (SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	ApplyPostProcess(ENetherCrownPPType::Charging, 2.5f, false);
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

	if (!LastDashAttackAnimMontageSoft.IsNull())
	{
		CachedLastDashAttackAnimMontage = LastDashAttackAnimMontageSoft.LoadSynchronous();
	}
}

TArray<AActor*> UNetherCrownSkillDashAttack::DetectDashAttackTargets() const
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || !SkillOwnerCharacter->HasAuthority())
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

	if (OverlappedActors.Num() > MaxTargetNum)
	{
		OverlappedActors.RemoveAt(MaxTargetNum, OverlappedActors.Num() - MaxTargetNum);
	}

	return OverlappedActors;
}

void UNetherCrownSkillDashAttack::DashAttackToTargets()
{
	if (CachedTargetActors.IsEmpty())
	{
		return;
	}

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	if (CachedTargetActors.IsValidIndex(CurrentTargetIndex))
	{
		CurrentTargetActor = CachedTargetActors[CurrentTargetIndex];
	}
	else
	{
		LastDashAttack();
		return;
	}

	++CurrentTargetIndex;

	if (!IsValid(CurrentTargetActor))
	{
		ClearDashAttackData();
		UE_LOG(LogNetherCrown, Warning, TEXT("Current Target Actor is InValid! : %hs"), __FUNCTION__);
		return;
	}

	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		ClearDashAttackData();
		UE_LOG(LogNetherCrown, Warning, TEXT("SkillOwnerCharacter is InValid! : %hs"), __FUNCTION__);
		return;
	}

	World->GetTimerManager().SetTimer(DashAttackHitTimerHandle, this, &ThisClass::HitDashAttack, DashDuration, false, 0.f);

	const FVector CurrentTargetLocation{ CurrentTargetActor->GetActorLocation() };
	const FVector SkillOwnerLocation{ SkillOwnerCharacter->GetActorLocation() };

	const FRotator RotationToTarget{ UKismetMathLibrary::FindLookAtRotation(SkillOwnerLocation, CurrentTargetLocation) };
	Multicast_SetOwnerCharacterRotToTarget(RotationToTarget); //@NOTE : Need Multicast RPC to rotate in dedicate server env

	float CurrentTargetCapsuleRadius{};
	const ANetherCrownEnemy* CurrentTargetEnemy{ Cast<ANetherCrownEnemy>(CurrentTargetActor) };
	const UCapsuleComponent* CurrentTargetEnemyCapsule{ CurrentTargetEnemy ? CurrentTargetEnemy->GetCapsuleComponent() : nullptr };
	if (IsValid(CurrentTargetEnemyCapsule))
	{
		CurrentTargetCapsuleRadius = CurrentTargetEnemyCapsule->GetScaledCapsuleRadius();
	}

	const FVector DashOffsetByDirection{ SkillOwnerCharacter->GetActorForwardVector() * (CurrentTargetCapsuleRadius + EndLocationOffset) };
	const FVector DashEndLocation{ CurrentTargetLocation + DashOffsetByDirection };
	Multicast_DashOwnerCharacter(SkillOwnerLocation, DashEndLocation); //@NOTE : Server에서만 호출 시 네트워크 복제 지연으로 끊김 발생
}

void UNetherCrownSkillDashAttack::LastDashAttack()
{
	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || !SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	World->GetTimerManager().ClearTimer(DashAttackTimerHandle);
	World->GetTimerManager().ClearTimer(DashAttackHitTimerHandle);

	Client_SetCameraViewLastDashAttack();
	Multicast_PlayLastDashAttackMontage();
	SetAttackLastDashAttackTimer();
}

void UNetherCrownSkillDashAttack::Multicast_DeactivateDashAttackGhostTrail_Implementation()
{
	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || SkillOwnerCharacter->HasAuthority())
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

void UNetherCrownSkillDashAttack::Client_ActiveSkillHitCameraShake_Implementation() const
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	ANetherCrownPlayerController* SkillOwnerController{ Cast<ANetherCrownPlayerController>(SkillOwnerCharacter->GetController()) };
	APlayerCameraManager* CameraManager{ SkillOwnerController ? SkillOwnerController->PlayerCameraManager : nullptr };
	if (!ensureAlways(IsValid(CameraManager)))
	{
		return;
	}

	CameraManager->StartCameraShake(DashAttackHitCameraShakeClass, 1.f);
}

void UNetherCrownSkillDashAttack::Client_StartPostProcessBlendEndTimer_Implementation()
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	UNetherCrownControlPPComponent* ControlPPComponent{ SkillOwnerCharacter->GetControlPPComponent() };
	if (!ensureAlways(IsValid(ControlPPComponent)))
	{
		return;
	}

	ControlPPComponent->StartSetPostProcessBlendEndTimeline();
}

void UNetherCrownSkillDashAttack::Client_SetCameraViewLastDashAttack_Implementation()
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	FActorSpawnParameters SpawnParameters{};
	SpawnParameters.ObjectFlags = RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FVector LastDashAttackCameraLocation{ SkillOwnerCharacter->GetActorLocation() + (SkillOwnerCharacter->GetActorForwardVector() * LastDashAttackCameraDistance) };
	const FRotator LastDashAttackCameraRotation{ UKismetMathLibrary::FindLookAtRotation(LastDashAttackCameraLocation, SkillOwnerCharacter->GetActorLocation()) };
	ACameraActor* DashAttackFinishCameraActor{ World->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), LastDashAttackCameraLocation, LastDashAttackCameraRotation, SpawnParameters) };
	if (!ensureAlways(IsValid(DashAttackFinishCameraActor)))
	{
		return;
	}

	ANetherCrownPlayerController* SkillOwnerController{ Cast<ANetherCrownPlayerController>(SkillOwnerCharacter->GetController()) };
	if (!ensureAlways(IsValid(SkillOwnerController)))
	{
		DashAttackFinishCameraActor->Destroy();
		return;
	}

	AActor* PreviousViewTarget{ SkillOwnerController->GetViewTarget() };
	if (!ensureAlways(IsValid(PreviousViewTarget)))
	{
		DashAttackFinishCameraActor->Destroy();
		return;
	}

	UCameraComponent* LastDashAttackCameraComponent{ DashAttackFinishCameraActor->GetCameraComponent() };
	if (!ensureAlways(IsValid(LastDashAttackCameraComponent)))
	{
		DashAttackFinishCameraActor->Destroy();
		return;
	}

	APlayerCameraManager* CameraManager{ SkillOwnerController->PlayerCameraManager };
	if (!ensureAlways(IsValid(CameraManager)))
	{
		DashAttackFinishCameraActor->Destroy();
		return;
	}

	LastDashAttackCameraComponent->SetFieldOfView(CameraManager->GetFOVAngle());
	LastDashAttackCameraComponent->bConstrainAspectRatio = false;

	int32 ViewportSizeX{ 0 };
	int32 ViewportSizeY{ 0 };
	SkillOwnerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	if (ViewportSizeX > 0 && ViewportSizeY > 0)
	{
		LastDashAttackCameraComponent->SetAspectRatio(static_cast<float>(ViewportSizeX) / static_cast<float>(ViewportSizeY));
	}

	constexpr float CameraBlendTime{ 0.5f };
	constexpr float CameraBlendExp{ 2.f };
	SkillOwnerController->SetViewTargetWithBlend(DashAttackFinishCameraActor, CameraBlendTime, VTBlend_Cubic, CameraBlendExp, true);

	FTimerDelegate RestoreCameraTimerDelegate{};
	RestoreCameraTimerDelegate.BindWeakLambda(this, [DashAttackFinishCameraActor, SkillOwnerController, PreviousViewTarget, World]()
	{
		if (IsValid(SkillOwnerController) && IsValid(PreviousViewTarget))
		{
			SkillOwnerController->SetViewTargetWithBlend(PreviousViewTarget, CameraBlendTime, VTBlend_EaseInOut, CameraBlendExp, true);
		}

		if (IsValid(DashAttackFinishCameraActor))
		{
			DashAttackFinishCameraActor->Destroy();
		}
	});

	constexpr float RestoreCameraTimerDuration{ 2.f };
	World->GetTimerManager().ClearTimer(LastDashAttackCameraTimerHandle);
	World->GetTimerManager().SetTimer(LastDashAttackCameraTimerHandle, RestoreCameraTimerDelegate, RestoreCameraTimerDuration, false);
}

void UNetherCrownSkillDashAttack::Multicast_PlayLastDashAttackMontage_Implementation() const
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	if (!IsValid(CachedLastDashAttackAnimMontage))
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("AnimMontage is null in %hs"), __FUNCTION__);
		return;
	}

	PlayDashAttackMontage(CachedLastDashAttackAnimMontage);
}

void UNetherCrownSkillDashAttack::SetAttackLastDashAttackTimer()
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || !SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	constexpr float LastDashAttackTimeOffset{ 1.f };
	World->GetTimerManager().ClearTimer(LastDashAttackHitStartTimerHandle);
	World->GetTimerManager().SetTimer(LastDashAttackHitStartTimerHandle, this, &ThisClass::AttackLastDashAttack, LastDashAttackTimeOffset, false);
}

void UNetherCrownSkillDashAttack::AttackLastDashAttack()
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || !SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	for (AActor* CachedTargetActor : CachedTargetActors)
	{
		if (ANetherCrownEnemy* TargetEnemy = Cast<ANetherCrownEnemy>(CachedTargetActor))
		{
			ApplyDashAttackDamageAndCrowdControl(TargetEnemy);
		}
	}

	ClearDashAttackData();
	Client_StartPostProcessBlendEndTimer();
}

void UNetherCrownSkillDashAttack::PlayDashAttackMontage(UAnimMontage* SkillAnimMontage) const
{
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	const USkeletalMeshComponent* SkeletalMeshComponent{ SkillOwnerCharacter->GetMesh() };
	UNetherCrownCharacterAnimInstance* NetherCrownCharacterAnimInstance{};
	NetherCrownCharacterAnimInstance = SkeletalMeshComponent ? Cast<UNetherCrownCharacterAnimInstance>(SkeletalMeshComponent->GetAnimInstance()) : nullptr;
	if (!ensureAlways(IsValid(NetherCrownCharacterAnimInstance)))
	{
		return;
	}

	if (!ensureAlways(IsValid(SkillAnimMontage)))
	{
		return;
	}

	NetherCrownCharacterAnimInstance->Montage_Play(SkillAnimMontage);
}

void UNetherCrownSkillDashAttack::HitDashAttack()
{
	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || !SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	ANetherCrownEnemy* CurrentTargetEnemy{ Cast<ANetherCrownEnemy>(CurrentTargetActor) };
	if (!ensureAlways(IsValid(CurrentTargetEnemy)))
	{
		return;
	}

	ApplyDashAttackDamageAndCrowdControl(CurrentTargetEnemy);
}

void UNetherCrownSkillDashAttack::ApplyDashAttackDamageAndCrowdControl(const ANetherCrownEnemy* TargetEnemy)
{
	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || !SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	if (!ensureAlways(IsValid(TargetEnemy)))
	{
		return;
	}

	UNetherCrownCrowdControlComponent* CrowdControlComponent{ TargetEnemy->GetCrowdControlComponent() };
	if (!ensureAlways(IsValid(CrowdControlComponent)))
	{
		return;
	}

	CrowdControlComponent->Stun();

	ApplyCrowdControlToTarget(TargetEnemy, ENetherCrownCrowdControlType::STUN, StunDuration);
	UGameplayStatics::ApplyDamage(CurrentTargetActor, CalculatePhysicalSkillDamage(), SkillOwnerCharacter->GetController(), SkillOwnerCharacter, UDamageType::StaticClass());

	Multicast_SpawnSkillImpactEffect(TargetEnemy);
	Client_ActiveSkillHitCameraShake();
}

void UNetherCrownSkillDashAttack::ClearDashAttackData()
{
	const UWorld* World{ GetWorld() };
	check(World);

	CachedTargetActors.Empty();
	CurrentTargetActor = nullptr;

	World->GetTimerManager().ClearTimer(DashAttackTimerHandle);
	World->GetTimerManager().ClearTimer(DashAttackHitTimerHandle);

	CurrentTargetIndex = 0;

	Multicast_DeactivateDashAttackGhostTrail();
	Multicast_SetCharacterCapsuleCollisionData(false);
}

void UNetherCrownSkillDashAttack::Multicast_SetCharacterCapsuleCollisionData_Implementation(const bool bStartDashAttack)
{
	//@NOTE : Server에서만 실행 시 Collision 변경 지연으로 살짝 부딪히는 느낌이 발생함
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
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
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	const UNetherCrownControlGhostTrailComponent* ControlGhostTrailComponent{ SkillOwnerCharacter->GetControlGhostTrailComponent() };
	if (!ensureAlways(ControlGhostTrailComponent))
	{
		return;
	}
	ControlGhostTrailComponent->ActivateGhostTrail(true, GhostTrailNiagaraSystem);

	if (IsValid(CachedSkillAnimMontage))
	{
		PlayDashAttackMontage(CachedSkillAnimMontage);
	}

	UCharacterMovementComponent* SkillOwnerCharacterMovementComponent{ SkillOwnerCharacter->GetCharacterMovement() };
	check(SkillOwnerCharacterMovementComponent);

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
	SkillOwnerCharacterMovementComponent->ApplyRootMotionSource(MoveToForce);
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
	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)) || !SkillOwnerCharacter->HasAuthority())
	{
		return;
	}

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	ClearDashAttackData();
	Multicast_SetCharacterCapsuleCollisionData(true);

	CachedTargetActors = DetectDashAttackTargets();
	if (CachedTargetActors.IsEmpty())
	{
		return;
	}

	World->GetTimerManager().SetTimer(DashAttackTimerHandle, this, &ThisClass::DashAttackToTargets, DashTimerRate, true, 0.f);
}
