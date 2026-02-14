// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownShield.h"

#include "NetherCrown/Util/NetherCrownCurveTimerUtil.h"

ANetherCrownShield::ANetherCrownShield()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	ShieldMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMeshComponent"));
	ShieldMeshComponent->SetupAttachment(RootComponent);

	bReplicates = false;
	SetReplicatingMovement(false);
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
	ensureAlways(IsValid(ShieldDynamicMaterialInstance));
}

void ANetherCrownShield::StartShieldMaterialCurveTimer(const bool bStartShield)
{
	const UWorld* World{ GetWorld() };
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
	FNetherCrownCurveTimerData CurveTimerData{};
	CurveTimerData.WorldContextObject = this;
	CurveTimerData.Curve = CachedStartCurveFloat;
	CurveTimerData.TimerHandle = &ShieldMaterialCurveTimerHandle;
	CurveTimerData.CurveElapsedTime = &ShieldMaterialCurveElapsedTime;
	CurveTimerData.CurveElapsedTimeOffset = 0.015f;
	CurveTimerData.CallBack = [WeakThis = MakeWeakObjectPtr(this)]() { WeakThis->SetBeginShieldDynamicMaterialScalarParam(); };

	FNetherCrownCurveTimerUtil::BeginTimerByFloatCurve(CurveTimerData);
}

void ANetherCrownShield::ApplyEndShieldMaterialParameterCurveFloat()
{
	FNetherCrownCurveTimerData CurveTimerData{};
	CurveTimerData.WorldContextObject = this;
	CurveTimerData.Curve = CachedEndCurveFloat;
	CurveTimerData.TimerHandle = &ShieldMaterialCurveTimerHandle;
	CurveTimerData.CurveElapsedTime = &ShieldMaterialCurveElapsedTime;
	CurveTimerData.CurveElapsedTimeOffset = 0.015f;
	CurveTimerData.CallBack = [WeakThis = MakeWeakObjectPtr(this)]() { WeakThis->SetEndShieldDynamicMaterialScalarParam(); };

	FNetherCrownCurveTimerUtil::BeginTimerByFloatCurve(CurveTimerData);
}

void ANetherCrownShield::SetBeginShieldDynamicMaterialScalarParam() const
{
	if (!ensureAlways(IsValid(ShieldDynamicMaterialInstance)))
	{
		return;
	}

	const float CurveValue{ CachedStartCurveFloat->GetFloatValue(ShieldMaterialCurveElapsedTime) };
	ShieldDynamicMaterialInstance->SetScalarParameterValue(ShieldMaterialScalarParameterName, CurveValue);
}

void ANetherCrownShield::SetEndShieldDynamicMaterialScalarParam()
{
	if (!ensureAlways(IsValid(ShieldDynamicMaterialInstance)))
	{
		return;
	}

	const float CurveValue{ CachedEndCurveFloat->GetFloatValue(ShieldMaterialCurveElapsedTime) };
	ShieldDynamicMaterialInstance->SetScalarParameterValue(ShieldMaterialScalarParameterName, CurveValue);

	if (FMath::IsNearlyZero(CurveValue))
	{
		Destroy();
	}
}
