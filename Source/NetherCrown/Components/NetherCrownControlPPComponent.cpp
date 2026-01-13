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

void UNetherCrownControlPPComponent::ApplyPostProcess(const ENetherCrownPPType PPType, float Duration)
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

	HandledPostProcessComponent->Settings = *FindProcessSettings;

	StartPostProcessBlendStartTimer();
	StartClearPostProcessTimer(Duration);
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

void UNetherCrownControlPPComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UNetherCrownControlPPComponent::StartClearPostProcessTimer(float Duration)
{
	const UWorld* World{ GetWorld() };
	check(World);

	FTimerHandle TimerHandle{};

	World->GetTimerManager().SetTimer(TimerHandle, this, &UNetherCrownControlPPComponent::StartPostProcessBlendEndTimer, Duration, false);
}

void UNetherCrownControlPPComponent::ClearPostProcess()
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
	const UWorld* World{ GetWorld() };
	check(World);

	UPostProcessComponent* HandledPostProcessComponent{ HandledPostProcessComponentWeak.Get() };
	if (!ensureAlways(IsValid(HandledPostProcessComponent)) || !ensureAlways(IsValid(CachedPostProcessStartCurveFloat)))
	{
		World->GetTimerManager().ClearTimer(PostProcessBlendStartTimerHandle);
		return;
	}

	PostProcessBlendElapsedTime += 0.01f;

	float Min{};
	float Max{};
	CachedPostProcessStartCurveFloat->GetTimeRange(Min, Max);

	if (PostProcessBlendElapsedTime > Max)
	{
		World->GetTimerManager().ClearTimer(PostProcessBlendStartTimerHandle);
		return;
	}

	HandledPostProcessComponent->BlendWeight = CachedPostProcessStartCurveFloat->GetFloatValue(PostProcessBlendElapsedTime);
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
	const UWorld* World{ GetWorld() };
	check(World);

	UPostProcessComponent* HandledPostProcessComponent{ HandledPostProcessComponentWeak.Get() };
	if (!ensureAlways(IsValid(HandledPostProcessComponent)) || !ensureAlways(IsValid(CachedPostProcessEndCurveFloat)))
	{
		World->GetTimerManager().ClearTimer(PostProcessBlendEndTimerHandle);
		return;
	}

	PostProcessBlendElapsedTime += 0.01f;

	float Min{};
	float Max{};
	CachedPostProcessEndCurveFloat->GetTimeRange(Min, Max);

	if (PostProcessBlendElapsedTime > Max)
	{
		ClearPostProcess();

		World->GetTimerManager().ClearTimer(PostProcessBlendEndTimerHandle);
		return;
	}

	HandledPostProcessComponent->BlendWeight = CachedPostProcessEndCurveFloat->GetFloatValue(PostProcessBlendElapsedTime);
}
