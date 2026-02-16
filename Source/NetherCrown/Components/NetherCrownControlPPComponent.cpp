// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownControlPPComponent.h"

#include "Components/PostProcessComponent.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Util/NetherCrownCurveTimerUtil.h"

UNetherCrownControlPPComponent::UNetherCrownControlPPComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UNetherCrownControlPPComponent::SetHandlingPostProcessComponent(UPostProcessComponent* PostProcessComponent)
{
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
	const ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)) || OwnerCharacter->HasAuthority() || !OwnerCharacter->IsLocallyControlled())
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

	StartPostProcessBlendStartTimer();

	if (bEndTimerAutomatic)
	{
		StartClearPostProcessTimer(Duration);
	}
}

void UNetherCrownControlPPComponent::ClearPostProcessImmediately()
{
	const UWorld* World{ GetWorld() };
	check(World);

	World->GetTimerManager().ClearTimer(PostProcessBlendStartTimerHandle);
	World->GetTimerManager().ClearTimer(PostProcessBlendEndTimerHandle);

	ResetPostProcess();
}

void UNetherCrownControlPPComponent::BeginPlay()
{
	Super::BeginPlay();

	const ANetherCrownCharacter* OwnerCharacter{ Cast<ANetherCrownCharacter>(GetOwner()) };
	if (!ensureAlways(IsValid(OwnerCharacter)) || OwnerCharacter->HasAuthority() || !OwnerCharacter->IsLocallyControlled())
	{
		return;
	}

	CachedPostProcessStartCurveFloat = PostProcessStartCurveFloatSoft.LoadSynchronous();
	CachedPostProcessEndCurveFloat = PostProcessEndCurveFloatSoft.LoadSynchronous();
}

void UNetherCrownControlPPComponent::StartClearPostProcessTimer(float Duration)
{
	const UWorld* World{ GetWorld() };
	check(World);

	FTimerHandle TimerHandle{};

	World->GetTimerManager().SetTimer(TimerHandle, this, &UNetherCrownControlPPComponent::StartPostProcessBlendEndTimer, Duration, false);
}

void UNetherCrownControlPPComponent::ResetPostProcess()
{
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

void UNetherCrownControlPPComponent::StartPostProcessBlendStartTimer()
{
	const UWorld* World{ GetWorld() };
	check(World);

	PostProcessBlendElapsedTime = 0.f;

	World->GetTimerManager().SetTimer(PostProcessBlendStartTimerHandle, this, &ThisClass::ApplyPostProcessBlendStartFloat, 0.01f, true, 0.f);
}

void UNetherCrownControlPPComponent::ApplyPostProcessBlendStartFloat()
{
	FNetherCrownCurveTimerData CurveTimerData{};
	CurveTimerData.WorldContextObject = this;
	CurveTimerData.Curve = CachedPostProcessStartCurveFloat;
	CurveTimerData.TimerHandle = &PostProcessBlendStartTimerHandle;
	CurveTimerData.CurveElapsedTime = &PostProcessBlendElapsedTime;
	CurveTimerData.CurveElapsedTimeOffset = 0.01f;
	CurveTimerData.CallBack = [WeakThis = MakeWeakObjectPtr(this)]() { WeakThis->SetBeginPostProcessBlendWeight(); };

	FNetherCrownCurveTimerUtil::ExecuteLoopTimerCallbackByCurve(CurveTimerData);
}

void UNetherCrownControlPPComponent::StartPostProcessBlendEndTimer()
{
	const UWorld* World{ GetWorld() };
	check(World);

	PostProcessBlendElapsedTime = 0.f;

	World->GetTimerManager().SetTimer(PostProcessBlendEndTimerHandle, this, &ThisClass::ApplyPostProcessBlendEndFloat, 0.01f, true, 0.f);
}

void UNetherCrownControlPPComponent::ApplyPostProcessBlendEndFloat()
{
	FNetherCrownCurveTimerData CurveTimerData{};
	CurveTimerData.WorldContextObject = this;
	CurveTimerData.Curve = CachedPostProcessEndCurveFloat;
	CurveTimerData.TimerHandle = &PostProcessBlendEndTimerHandle;
	CurveTimerData.CurveElapsedTime = &PostProcessBlendElapsedTime;
	CurveTimerData.CurveElapsedTimeOffset = 0.01f;
	CurveTimerData.CallBack = [WeakThis = MakeWeakObjectPtr(this)]() { WeakThis->SetEndPostProcessBlendWeight(); };
	CurveTimerData.ClearCallBack = [WeakThis = MakeWeakObjectPtr(this)]() { WeakThis->ResetPostProcess(); };

	FNetherCrownCurveTimerUtil::ExecuteLoopTimerCallbackByCurve(CurveTimerData);
}

void UNetherCrownControlPPComponent::SetBeginPostProcessBlendWeight() const
{
	UPostProcessComponent* HandledPostProcessComponent{ HandledPostProcessComponentWeak.Get() };
	if (!ensureAlways(IsValid(HandledPostProcessComponent)) || !ensureAlways(IsValid(CachedPostProcessStartCurveFloat)))
	{
		return;
	}

	HandledPostProcessComponent->BlendWeight = CachedPostProcessStartCurveFloat->GetFloatValue(PostProcessBlendElapsedTime);
}

void UNetherCrownControlPPComponent::SetEndPostProcessBlendWeight() const
{
	UPostProcessComponent* HandledPostProcessComponent{ HandledPostProcessComponentWeak.Get() };
	if (!ensureAlways(IsValid(HandledPostProcessComponent)) || !ensureAlways(IsValid(CachedPostProcessEndCurveFloat)))
	{
		return;
	}

	HandledPostProcessComponent->BlendWeight = CachedPostProcessEndCurveFloat->GetFloatValue(PostProcessBlendElapsedTime);
}
