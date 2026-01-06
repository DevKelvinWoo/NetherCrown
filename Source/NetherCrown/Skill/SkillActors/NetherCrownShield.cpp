// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownShield.h"

ANetherCrownShield::ANetherCrownShield()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	ShieldMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMeshComponent"));
	ShieldMeshComponent->SetupAttachment(RootComponent);

	bNetLoadOnClient = true;
	bReplicates = true;
	SetReplicatingMovement(true);
}

void ANetherCrownShield::DestroyShield()
{
	StartShieldMaterialCurveTimer(false);
}

void ANetherCrownShield::BeginPlay()
{
	Super::BeginPlay();

	CachedStartCurveFloat = ShieldMaterialStartCurveFloatSoft.LoadSynchronous();
	CachedEndCurveFloat = ShieldMaterialEndCurveFloatSoft.LoadSynchronous();

	CreateShieldDynamicMaterialInstance();

	StartShieldMaterialCurveTimer(true);
}

void ANetherCrownShield::CreateShieldDynamicMaterialInstance()
{
	ShieldDynamicMaterialInstance = ShieldMeshComponent->CreateDynamicMaterialInstance(0);
	if (!ensureAlways(IsValid(ShieldDynamicMaterialInstance)))
	{
		return;
	}
}

void ANetherCrownShield::StartShieldMaterialCurveTimer(const bool bStartShield)
{
	UWorld* World{ GetWorld() };
	check(World);

	ShieldMaterialCurveElapsedTime = 0.f;

	if (bStartShield)
	{
		World->GetTimerManager().SetTimer(
			ShieldMaterialCurveTimerHandle,
			this,
			&ThisClass::ApplyBeginShieldMaterialParameterCurveFloat,
			0.015f,
			true);
	}
	else
	{
		World->GetTimerManager().SetTimer(
			ShieldMaterialCurveTimerHandle,
			this,
			&ThisClass::ApplyEndShieldMaterialParameterCurveFloat,
			0.015f,
			true);
	}
}

void ANetherCrownShield::ApplyBeginShieldMaterialParameterCurveFloat()
{
	const UWorld* World{ GetWorld() };
	check(World);

	if (!ensureAlways(IsValid(CachedStartCurveFloat)))
	{
		World->GetTimerManager().ClearTimer(ShieldMaterialCurveTimerHandle);
		return;
	}

	ShieldMaterialCurveElapsedTime += 0.015f;

	float Min{};
	float Max{};
	CachedStartCurveFloat->GetTimeRange(Min, Max);

	if (ShieldMaterialCurveElapsedTime > Max)
	{
		World->GetTimerManager().ClearTimer(ShieldMaterialCurveTimerHandle);
		return;
	}

	if (!ensureAlways(IsValid(ShieldDynamicMaterialInstance)))
	{
		return;
	}

	const float CurveValue{ CachedStartCurveFloat->GetFloatValue(ShieldMaterialCurveElapsedTime) };
	ShieldDynamicMaterialInstance->SetScalarParameterValue(ShieldMaterialScalarParameterName, CurveValue);
}

void ANetherCrownShield::ApplyEndShieldMaterialParameterCurveFloat()
{
	const UWorld* World{ GetWorld() };
	check(World);

	if (!ensureAlways(IsValid(CachedEndCurveFloat)))
	{
		World->GetTimerManager().ClearTimer(ShieldMaterialCurveTimerHandle);
		return;
	}

	ShieldMaterialCurveElapsedTime += 0.015f;

	float Min{};
	float Max{};
	CachedEndCurveFloat->GetTimeRange(Min, Max);

	if (ShieldMaterialCurveElapsedTime > Max)
	{
		World->GetTimerManager().ClearTimer(ShieldMaterialCurveTimerHandle);
		return;
	}

	if (!ensureAlways(IsValid(ShieldDynamicMaterialInstance)))
	{
		return;
	}

	const float CurveValue{ CachedEndCurveFloat->GetFloatValue(ShieldMaterialCurveElapsedTime) };
	ShieldDynamicMaterialInstance->SetScalarParameterValue(ShieldMaterialScalarParameterName, CurveValue);

	if (FMath::IsNearlyZero(CurveValue))
	{
		Destroy();
		return;
	}
}
