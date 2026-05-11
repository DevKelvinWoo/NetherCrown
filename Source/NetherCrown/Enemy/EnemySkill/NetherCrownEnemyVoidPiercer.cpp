// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyVoidPiercer.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Enemy/NetherCrownEnemy.h"
#include "NetherCrown/Tags/NetherCrownGameplayTags.h"
#include "NetherCrown/Util/NetherCrownUtilManager.h"

void UNetherCrownEnemyVoidPiercer::InitEnemySkillObject()
{
	Super::InitEnemySkillObject();

	CacheVoidPiercerData();
}

void UNetherCrownEnemyVoidPiercer::PlayEnemySkillCosmetics()
{
	Super::PlayEnemySkillCosmetics();
}

void UNetherCrownEnemyVoidPiercer::ExecuteEnemySkillGameplay()
{
	Super::ExecuteEnemySkillGameplay();

	JumpAndFlyToUseSkill();
}

void UNetherCrownEnemyVoidPiercer::StartVoidPiercerTraceTimer()
{
	const ANetherCrownEnemy* SkillOwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerEnemy)) || !SkillOwnerEnemy->HasAuthority())
	{
		FinishEnemySkill();
		return;
	}

	AttackVoidPiercerToCharacter();

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		FinishEnemySkill();
		return;
	}

	FTimerManager& TimerManager{ World->GetTimerManager() };
	TimerManager.ClearTimer(TraceTimerHandle);
	TimerManager.SetTimer(TraceTimerHandle, this, &ThisClass::AttackVoidPiercerToCharacter, CachedVoidPiercerData.DamageInterval, true);
}

void UNetherCrownEnemyVoidPiercer::AttackVoidPiercerToCharacter()
{
	ANetherCrownEnemy* SkillOwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerEnemy)) || !SkillOwnerEnemy->HasAuthority())
	{
		FinishEnemySkill();
		return;
	}

	if (!bCanActiveTrace)
	{
		return;
	}

	USkeletalMeshComponent* SkillOwnerEnemySkeletalMeshComponent{ SkillOwnerEnemy->GetMesh() };
	if (!ensureAlways(IsValid(SkillOwnerEnemySkeletalMeshComponent)))
	{
		return;
	}

	const FVector LeftHandSocketLocation{ SkillOwnerEnemySkeletalMeshComponent->GetSocketLocation(TEXT("hand_l")) };
	const FVector RightHandSocketLocation{ SkillOwnerEnemySkeletalMeshComponent->GetSocketLocation(TEXT("hand_r")) };

	const FVector StartLocation{ (LeftHandSocketLocation + RightHandSocketLocation) / 2.f };
	const FVector Direction{ SkillOwnerEnemy->GetActorForwardVector() };
	const FVector EndLocation{ StartLocation + Direction * CachedVoidPiercerData.LaserRange };

	TArray<AActor*> ActorsToIgnore{};
	ActorsToIgnore.Add(SkillOwnerEnemy);

	TArray<FHitResult> OutHits{};
	UKismetSystemLibrary::SphereTraceMulti(
		  GetWorld(),
		  StartLocation,
		  EndLocation,
		  CachedVoidPiercerData.LaserRadius,
		  UEngineTypes::ConvertToTraceType(ECC_Visibility),
		  false,
		  ActorsToIgnore,
		  EDrawDebugTrace::None,
		  OutHits,
		  true
	);

	for (const FHitResult& Hit : OutHits)
	{
		AActor* HitActor{ Hit.GetActor() };
		if (IsValid(HitActor))
		{
			Multicast_PlayVoidPiercerHitImpactEffect(Hit.ImpactPoint);
		}

		ANetherCrownCharacter* HitCharacter{ Cast<ANetherCrownCharacter>(HitActor) };
		if (!IsValid(HitCharacter))
		{
			continue;
		}

		ApplyEnemyMagicSkillDamage(HitCharacter);
	}
}

void UNetherCrownEnemyVoidPiercer::CacheVoidPiercerData()
{
	const UNetherCrownEnemySkillDataAsset* EnemySkillDataAsset{ FNetherCrownUtilManager::GetEnemySkillDataAssetByGameplayTag(NetherCrownTags::Enemy_Skill_VoidPiercer) };
	const UNetherCrownEnemyVoidPiercerDataAsset* EnemyVoidPiercerDataAsset{ Cast<UNetherCrownEnemyVoidPiercerDataAsset>(EnemySkillDataAsset) };
	if (!ensureAlways(IsValid(EnemyVoidPiercerDataAsset)))
	{
		return;
	}

	CachedVoidPiercerData = EnemyVoidPiercerDataAsset->GetEnemyVoidPiercerData();
}

void UNetherCrownEnemyVoidPiercer::JumpAndFlyToUseSkill()
{
	const ANetherCrownEnemy* SkillOwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerEnemy)) || !SkillOwnerEnemy->HasAuthority())
	{
		FinishEnemySkill();
		return;
	}

	const FVector& StartLocation{ SkillOwnerEnemy->GetActorLocation() };
	const FVector& JumpTargetLocation{ StartLocation + SkillOwnerEnemy->GetActorUpVector() * CachedVoidPiercerData.JumpToSkyOffset };

	TSharedPtr<FRootMotionSource_MoveToForce> MoveToForceRootMotionSource{ MakeShared<FRootMotionSource_MoveToForce>() };
	MoveToForceRootMotionSource->InstanceName = TEXT("EnemyVoidPiercer");
	MoveToForceRootMotionSource->AccumulateMode = ERootMotionAccumulateMode::Override;
	MoveToForceRootMotionSource->Priority = 5;
	MoveToForceRootMotionSource->StartLocation = StartLocation;
	MoveToForceRootMotionSource->TargetLocation = JumpTargetLocation;
	MoveToForceRootMotionSource->Duration = CachedVoidPiercerData.JumpToSkyDuration;
	MoveToForceRootMotionSource->bRestrictSpeedToExpected = true;

	MoveToForceRootMotionSource->FinishVelocityParams.Mode = ERootMotionFinishVelocityMode::SetVelocity;
	MoveToForceRootMotionSource->FinishVelocityParams.SetVelocity = FVector::ZeroVector;

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		FinishEnemySkill();
		return;
	}

	FTimerManager& TimerManager{ World->GetTimerManager() };
	TimerManager.ClearTimer(FlyTimerHandle);
	TimerManager.SetTimer(FlyTimerHandle, this, &ThisClass::HandleVoidPiercerJumpFinished, CachedVoidPiercerData.JumpToSkyDuration, false);

	UCharacterMovementComponent* SkillOwnerEnemyMovementComponent{ SkillOwnerEnemy->GetCharacterMovement() };
	if (!ensureAlways(IsValid(SkillOwnerEnemyMovementComponent)))
	{
		FinishEnemySkill();
		return;
	}
	SkillOwnerEnemyMovementComponent->ApplyRootMotionSource(MoveToForceRootMotionSource);
}

void UNetherCrownEnemyVoidPiercer::RotateToTargetActor()
{
	ANetherCrownEnemy* SkillOwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerEnemy)) || !SkillOwnerEnemy->HasAuthority())
	{
		FinishEnemySkill();
		return;
	}

	const ANetherCrownCharacter* TargetCharacter{ SkillOwnerEnemy->GetCurrentTargetCharacter() };
	if (!IsValid(TargetCharacter))
	{
		return;
	}

	const FRotator& LookAtRotation{ UKismetMathLibrary::FindLookAtRotation(SkillOwnerEnemy->GetActorLocation(), TargetCharacter->GetActorLocation()) };
	SkillOwnerEnemy->SetActorRotation(LookAtRotation);
}

void UNetherCrownEnemyVoidPiercer::HandleVoidPiercerJumpFinished()
{
	const ANetherCrownEnemy* SkillOwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerEnemy)) || !SkillOwnerEnemy->HasAuthority())
	{
		FinishEnemySkill();
		return;
	}

	UCharacterMovementComponent* SkillOwnerEnemyMovementComponent{ SkillOwnerEnemy->GetCharacterMovement() };
	if (!ensureAlways(IsValid(SkillOwnerEnemyMovementComponent)))
	{
		FinishEnemySkill();
		return;
	}

	SkillOwnerEnemyMovementComponent->SetMovementMode(MOVE_Flying);

	bCanActiveTrace = true;
	StartVoidPiercerTraceTimer();

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		FinishEnemySkill();
		return;
	}

	FTimerManager& TimerManager{ World->GetTimerManager() };
	TimerManager.ClearTimer(LandTimerHandle);
	TimerManager.SetTimer(LandTimerHandle, this, &ThisClass::HandleVoidPiercerFlyFinished, CachedVoidPiercerData.FlyingDuration, false);

	RotateToTargetActor();
}

void UNetherCrownEnemyVoidPiercer::HandleVoidPiercerFlyFinished()
{
	ANetherCrownEnemy* SkillOwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerEnemy)) || !SkillOwnerEnemy->HasAuthority())
	{
		FinishEnemySkill();
		return;
	}

	UCharacterMovementComponent* SkillOwnerEnemyMovementComponent{ SkillOwnerEnemy->GetCharacterMovement() };
	if (!ensureAlways(IsValid(SkillOwnerEnemyMovementComponent)))
	{
		FinishEnemySkill();
		return;
	}

	bCanActiveTrace = false;

	const FRotator& SkillOwnerEnemyRotation{ SkillOwnerEnemy->GetActorRotation() };
	SkillOwnerEnemy->SetActorRotation(FRotator(0.f, SkillOwnerEnemyRotation.Yaw, SkillOwnerEnemyRotation.Roll));
	SkillOwnerEnemyMovementComponent->SetMovementMode(MOVE_Walking);
}

void UNetherCrownEnemyVoidPiercer::Multicast_PlayVoidPiercerHitImpactEffect_Implementation(const FVector& HitLocation)
{
	const ANetherCrownEnemy* SkillOwnerEnemy{ SkillOwnerEnemyWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerEnemy)) || SkillOwnerEnemy->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	const FGameplayTag& VoidPiercerHitImpactEffectTag{ CachedVoidPiercerData.VoidPiercerTagData.HitImpactEffectTag };
	const FTransform& HitImpactTransform{ FTransform(FRotator::ZeroRotator, HitLocation, FVector::OneVector) };
	FNetherCrownUtilManager::SpawnNiagaraSystemByGameplayTag(this, VoidPiercerHitImpactEffectTag, HitImpactTransform);
}
