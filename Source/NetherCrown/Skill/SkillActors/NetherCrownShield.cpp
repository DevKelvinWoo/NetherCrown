// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownShield.h"

ANetherCrownShield::ANetherCrownShield()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	ShieldMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMeshComponent"));
	ShieldMeshComponent->SetupAttachment(RootComponent);

	bReplicates = false;
	SetReplicatingMovement(false);
}

void ANetherCrownShield::DestroyShield()
{
	StartSetEndShieldMaterialTimeline();
}

void ANetherCrownShield::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	CachedStartCurveFloat = ShieldMaterialStartCurveFloatSoft.LoadSynchronous();
	CachedEndCurveFloat = ShieldMaterialEndCurveFloatSoft.LoadSynchronous();

	CreateShieldDynamicMaterialInstance();

	BindTimelineFunctions();

	StartSetBeginShieldMaterialTimeline();
}

void ANetherCrownShield::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (BeginShieldMaterialFloatTimeline.IsPlaying())
	{
		BeginShieldMaterialFloatTimeline.TickTimeline(DeltaTime);
	}

	if (EndShieldMaterialFloatTimeline.IsPlaying())
	{
		EndShieldMaterialFloatTimeline.TickTimeline(DeltaTime);
	}
}

void ANetherCrownShield::CreateShieldDynamicMaterialInstance()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	ShieldDynamicMaterialInstance = ShieldMeshComponent->CreateDynamicMaterialInstance(0);
	ensureAlways(IsValid(ShieldDynamicMaterialInstance));
}

void ANetherCrownShield::BindTimelineFunctions()
{
	FOnTimelineFloat BeginShieldMaterialProgressFunc{};
	BeginShieldMaterialProgressFunc.BindUFunction(this, FName("SetBeginShieldMaterialByFloatTimeline"));
	BeginShieldMaterialFloatTimeline.AddInterpFloat(CachedStartCurveFloat, BeginShieldMaterialProgressFunc);

	FOnTimelineFloat EndShieldMaterialProgressFunc{};
	EndShieldMaterialProgressFunc.BindUFunction(this, FName("SetEndShieldMaterialByFloatTimeline"));
	EndShieldMaterialFloatTimeline.AddInterpFloat(CachedEndCurveFloat, EndShieldMaterialProgressFunc);
}

void ANetherCrownShield::StartSetBeginShieldMaterialTimeline()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	BeginShieldMaterialFloatTimeline.PlayFromStart();
}

void ANetherCrownShield::StartSetEndShieldMaterialTimeline()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	EndShieldMaterialFloatTimeline.PlayFromStart();
}

void ANetherCrownShield::SetBeginShieldMaterialByFloatTimeline(float FloatCurveValue)
{
	if (!ensureAlways(IsValid(ShieldDynamicMaterialInstance)) || GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	ShieldDynamicMaterialInstance->SetScalarParameterValue(ShieldMaterialScalarParameterName, FloatCurveValue);
}

void ANetherCrownShield::SetEndShieldMaterialByFloatTimeline(float FloatCurveValue)
{
	if (!ensureAlways(IsValid(ShieldDynamicMaterialInstance)) || GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	ShieldDynamicMaterialInstance->SetScalarParameterValue(ShieldMaterialScalarParameterName, FloatCurveValue);

	if (FMath::IsNearlyZero(FloatCurveValue))
	{
		Destroy();
	}
}

