// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownSkillSkyFallSlash.h"

void UNetherCrownSkillSkyFallSlash::ExecuteSkillGameplay() const
{
	Super::ExecuteSkillGameplay();

	//@NOTE : Only gameplay logic (server)
}

void UNetherCrownSkillSkyFallSlash::PlaySkillCosmetics()
{
	Super::PlaySkillCosmetics();

	//@NOTE : Only cometics logic (all client)
	StartSkillCameraCurveTimer();
}

void UNetherCrownSkillSkyFallSlash::StartSkillCameraCurveTimer()
{
	ElapsedTime = 0.f;

	const UWorld* World{ GetWorld() };
	check(World);

	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	if (!SkillOwnerCharacter->IsLocallyControlled())
	{
		return;
	}

	World->GetTimerManager().SetTimer(
		SkillCameraCurveTimerHandle,
		this,
		&ThisClass::ApplySkillCameraCurveFloat,
		0.015f,
		true,
		0.f
	);
}

void UNetherCrownSkillSkyFallSlash::ApplySkillCameraCurveFloat()
{
	const UWorld* World{ GetWorld() };
	const ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	const UCurveFloat* SkillCameraCurveFloat{ SkillCameraCurveFloatSoft.LoadSynchronous() };
	if (!ensureAlways(IsValid(SkillCameraCurveFloat)) || !ensureAlways(IsValid(World)) || !ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		World->GetTimerManager().ClearTimer(SkillCameraCurveTimerHandle);
		return;
	}

	if (!SkillOwnerCharacter->IsLocallyControlled())
	{
		return;
	}

	ElapsedTime += 1.f;

	float MinTime{};
	float MaxTime{};
	SkillCameraCurveFloat->GetTimeRange(MinTime, MaxTime);

	if (ElapsedTime > MaxTime)
	{
		World->GetTimerManager().ClearTimer(SkillCameraCurveTimerHandle);
		return;
	}

	SkillOwnerCharacter->SetSpringArmZOffset(SkillCameraCurveFloat->GetFloatValue(ElapsedTime));
}
