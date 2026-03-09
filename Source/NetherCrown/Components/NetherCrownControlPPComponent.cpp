// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownControlPPComponent.h"

#include "Components/PostProcessComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"

UNetherCrownControlPPComponent::UNetherCrownControlPPComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UNetherCrownControlPPComponent::SetHandlingPostProcessComponent(UPostProcessComponent* PostProcessComponent)
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->IsLocallyControlled())
	{
		return;
	}

	HandledPostProcessComponentWeak = MakeWeakObjectPtr(PostProcessComponent);

	UPostProcessComponent* HandledPostProcessComponent{ HandledPostProcessComponentWeak.Get() };
	if (!ensureAlways(IsValid(HandledPostProcessComponent)))
	{
		return;
	}

	HandledPostProcessComponent->bUnbound = true;
	HandledPostProcessComponent->Priority = 10.f;
	HandledPostProcessComponent->BlendWeight = 0.f;
}

void UNetherCrownControlPPComponent::ApplyPostProcess(const ENetherCrownPPType PPType, float Duration, const bool bEndTimerAutomatic/*true*/)
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->IsLocallyControlled())
	{
		return;
	}

	UPostProcessComponent* HandledPostProcessComponent{ HandledPostProcessComponentWeak.Get() };
	if (!ensureAlways(IsValid(HandledPostProcessComponent)))
	{
		return;
	}

	FPostProcessSettings* FindProcessSettings{ PostProcessSettingsMap.Find(PPType) };
	if (!ensureAlways(FindProcessSettings))
	{
		return;
	}

	ClearPostProcessImmediately();

	HandledPostProcessComponent->Settings = *FindProcessSettings;

	StartSetPostProcessBlendStartTimeline();

	if (bEndTimerAutomatic)
	{
		StartClearPostProcessTimer(Duration);
	}
}

void UNetherCrownControlPPComponent::ClearPostProcessImmediately()
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->IsLocallyControlled())
	{
		return;
	}

	PostProcessBlendStartFloatTimeline.Stop();
	PostProcessBlendEndFloatTimeline.Stop();

	ResetPostProcess();
}

void UNetherCrownControlPPComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedCharacter = Cast<ANetherCrownCharacter>(GetOwner());
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->IsLocallyControlled())
	{
		return;
	}

	CachedPostProcessStartCurveFloat = PostProcessStartCurveFloatSoft.LoadSynchronous();
	CachedPostProcessEndCurveFloat = PostProcessEndCurveFloatSoft.LoadSynchronous();

	BindTimelineFunctions();
}

void UNetherCrownControlPPComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->IsLocallyControlled())
	{
		return;
	}

	if (PostProcessBlendStartFloatTimeline.IsPlaying())
	{
		PostProcessBlendStartFloatTimeline.TickTimeline(DeltaTime);
	}

	if (PostProcessBlendEndFloatTimeline.IsPlaying())
	{
		PostProcessBlendEndFloatTimeline.TickTimeline(DeltaTime);
	}
}

void UNetherCrownControlPPComponent::BindTimelineFunctions()
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->IsLocallyControlled())
	{
		return;
	}

	FOnTimelineFloat PostProcessBlendStartProgressFunc{};
	PostProcessBlendStartProgressFunc.BindUFunction(this, FName("SetPostProcessBlendStartByFloatTimeline"));
	PostProcessBlendStartFloatTimeline.AddInterpFloat(CachedPostProcessStartCurveFloat, PostProcessBlendStartProgressFunc);

	FOnTimelineFloat PostProcessBlendEndProgressFunc{};
	PostProcessBlendEndProgressFunc.BindUFunction(this, FName("SetPostProcessBlendEndByFloatTimeline"));
	PostProcessBlendEndFloatTimeline.AddInterpFloat(CachedPostProcessEndCurveFloat, PostProcessBlendEndProgressFunc);

	FOnTimelineEvent PostProcessBlendEndFinishedFunc{};
	PostProcessBlendEndFinishedFunc.BindUFunction(this, FName("HandlePostProcessBlendEndTimelineFinished"));
	PostProcessBlendEndFloatTimeline.SetTimelineFinishedFunc(PostProcessBlendEndFinishedFunc);
}

void UNetherCrownControlPPComponent::StartClearPostProcessTimer(float Duration)
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->IsLocallyControlled())
	{
		return;
	}

	const UWorld* World{ GetWorld() };
	check(World);

	FTimerHandle TimerHandle{};

	World->GetTimerManager().SetTimer(TimerHandle, this, &UNetherCrownControlPPComponent::StartSetPostProcessBlendEndTimeline, Duration, false);
}

void UNetherCrownControlPPComponent::ResetPostProcess()
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->IsLocallyControlled())
	{
		return;
	}

	UPostProcessComponent* HandledPostProcessComponent{ HandledPostProcessComponentWeak.Get() };
	if (!ensureAlways(IsValid(HandledPostProcessComponent)))
	{
		return;
	}

	FPostProcessSettings* DefaultProcessSettings{ PostProcessSettingsMap.Find(ENetherCrownPPType::Default) };
	if (!ensureAlways(DefaultProcessSettings))
	{
		return;
	}

	HandledPostProcessComponent->Settings = *DefaultProcessSettings;
}

void UNetherCrownControlPPComponent::StartSetPostProcessBlendStartTimeline()
{
	PostProcessBlendStartFloatTimeline.PlayFromStart();
}

void UNetherCrownControlPPComponent::StartSetPostProcessBlendEndTimeline()
{
	PostProcessBlendEndFloatTimeline.PlayFromStart();
}

void UNetherCrownControlPPComponent::SetPostProcessBlendStartByFloatTimeline(float FloatCurveValue)
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->IsLocallyControlled())
	{
		return;
	}

	UPostProcessComponent* HandledPostProcessComponent{ HandledPostProcessComponentWeak.Get() };
	if (!ensureAlways(IsValid(HandledPostProcessComponent)))
	{
		return;
	}

	HandledPostProcessComponent->BlendWeight = FloatCurveValue;
}

void UNetherCrownControlPPComponent::SetPostProcessBlendEndByFloatTimeline(float FloatCurveValue)
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->IsLocallyControlled())
	{
		return;
	}

	UPostProcessComponent* HandledPostProcessComponent{ HandledPostProcessComponentWeak.Get() };
	if (!ensureAlways(IsValid(HandledPostProcessComponent)))
	{
		return;
	}

	HandledPostProcessComponent->BlendWeight = FloatCurveValue;
}

void UNetherCrownControlPPComponent::HandlePostProcessBlendEndTimelineFinished()
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->IsLocallyControlled())
	{
		return;
	}

	ResetPostProcess();
}
