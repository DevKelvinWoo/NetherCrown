// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownControlPPComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/PostProcessComponent.h"
#include "Curves/CurveFloat.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"
#include "NetherCrown/Components/NetherCrownPlayerStatComponent.h"
#include "NetherCrown/PlayerState/NetherCrownPlayerState.h"

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

void UNetherCrownControlPPComponent::SetHandlingCameraComponent(UCameraComponent* CameraComponent)
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->IsLocallyControlled())
	{
		return;
	}

	HandledCameraComponentWeak = MakeWeakObjectPtr(CameraComponent);

	UCameraComponent* HandledCameraComponent{ HandledCameraComponentWeak.Get() };
	if (!ensureAlways(IsValid(HandledCameraComponent)))
	{
		return;
	}

	HandledCameraComponent->PostProcessBlendWeight = 0.f;
}

void UNetherCrownControlPPComponent::ApplyPostProcess(const ENetherCrownPPType PPType, const float Duration, const bool bEndTimerAutomatic/*true*/)
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

	FPostProcessSettings* FindProcessSettings{ PostProcessCosmeticData.PostProcessSettingsMap.Find(PPType) };
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

	if (const UWorld* World{ GetWorld() })
	{
		World->GetTimerManager().ClearTimer(TimerHandle_ClearPostProcess);
	}

	PostProcessBlendStartFloatTimeline.Stop();
	PostProcessBlendEndFloatTimeline.Stop();

	ResetPostProcess();
}

void UNetherCrownControlPPComponent::TryBindLowHealthPostProcess()
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->IsLocallyControlled())
	{
		return;
	}

	if (!ensureAlways(IsValid(HandledCameraComponentWeak.Get())))
	{
		return;
	}

	ANetherCrownPlayerState* NetherCrownPlayerState{ CachedCharacter->GetPlayerState<ANetherCrownPlayerState>() };
	if (!IsValid(NetherCrownPlayerState))
	{
		SetLowHealthCameraPostProcessActive(false);
		return;
	}

	UNetherCrownPlayerStatComponent* PlayerStatComponent{ NetherCrownPlayerState->GetNetherCrownPlayerStatComponent() };
	if (!ensureAlways(IsValid(PlayerStatComponent)))
	{
		SetLowHealthCameraPostProcessActive(false);
		return;
	}

	if (BoundPlayerStatComponent == PlayerStatComponent)
	{
		return;
	}

	if (IsValid(BoundPlayerStatComponent))
	{
		BoundPlayerStatComponent->GetOnCharacterHPModified().RemoveAll(this);
	}

	BoundPlayerStatComponent = PlayerStatComponent;
	BoundPlayerStatComponent->GetOnCharacterHPModified().AddUObject(this, &ThisClass::HandleCharacterHPModified);

	const FNetherCrownPlayerStat& PlayerStatData{ BoundPlayerStatComponent->GetPlayerStatData() };
	if (PlayerStatData.CharacterMaxHP <= 0.f)
	{
		SetLowHealthCameraPostProcessActive(false);
		return;
	}

	HandleCharacterHPModified(PlayerStatData.CharacterHP / PlayerStatData.CharacterMaxHP);
}

void UNetherCrownControlPPComponent::FlashCrowdControlPostProcess()
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->IsLocallyControlled())
	{
		return;
	}

	SetLowHealthCameraPostProcessActive(true);

	UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	World->GetTimerManager().ClearTimer(TimerHandle_CrowdControlPostProcessFlash);
	World->GetTimerManager().SetTimer(
		TimerHandle_CrowdControlPostProcessFlash,
		this,
		&ThisClass::RestorePostProcessAfterCrowdControlFlash,
		CrowdControlPostProcessFlashDuration,
		false
	);
}

void UNetherCrownControlPPComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedCharacter = Cast<ANetherCrownCharacter>(GetOwner());
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->IsLocallyControlled())
	{
		return;
	}

	LoadPostProcessCosmeticData();

	CachedPostProcessStartCurveFloat = PostProcessCosmeticData.PostProcessStartCurveFloatSoft.IsNull() ? nullptr : PostProcessCosmeticData.PostProcessStartCurveFloatSoft.LoadSynchronous();
	CachedPostProcessEndCurveFloat = PostProcessCosmeticData.PostProcessEndCurveFloatSoft.IsNull() ? nullptr : PostProcessCosmeticData.PostProcessEndCurveFloatSoft.LoadSynchronous();

	BindTimelineFunctions();
}

void UNetherCrownControlPPComponent::LoadPostProcessCosmeticData()
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->IsLocallyControlled())
	{
		return;
	}

	if (PostProcessCosmeticDataAssetSoft.IsNull())
	{
		return;
	}

	const UNetherCrownPostProcessCosmeticDataAsset* PostProcessCosmeticDataAsset{ PostProcessCosmeticDataAssetSoft.LoadSynchronous() };
	if (!ensureAlways(IsValid(PostProcessCosmeticDataAsset)))
	{
		return;
	}

	PostProcessCosmeticData = PostProcessCosmeticDataAsset->GetPostProcessCosmeticData();
}

void UNetherCrownControlPPComponent::TickComponent(const float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
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

	if (IsValid(CachedPostProcessStartCurveFloat))
	{
		FOnTimelineFloat PostProcessBlendStartProgressFunc{};
		PostProcessBlendStartProgressFunc.BindUFunction(this, FName("SetPostProcessBlendStartByFloatTimeline"));
		PostProcessBlendStartFloatTimeline.AddInterpFloat(CachedPostProcessStartCurveFloat, PostProcessBlendStartProgressFunc);
	}

	if (IsValid(CachedPostProcessEndCurveFloat))
	{
		FOnTimelineFloat PostProcessBlendEndProgressFunc{};
		PostProcessBlendEndProgressFunc.BindUFunction(this, FName("SetPostProcessBlendEndByFloatTimeline"));
		PostProcessBlendEndFloatTimeline.AddInterpFloat(CachedPostProcessEndCurveFloat, PostProcessBlendEndProgressFunc);
	}

	FOnTimelineEvent PostProcessBlendEndFinishedFunc{};
	PostProcessBlendEndFinishedFunc.BindUFunction(this, FName("HandlePostProcessBlendEndTimelineFinished"));
	PostProcessBlendEndFloatTimeline.SetTimelineFinishedFunc(PostProcessBlendEndFinishedFunc);
}

void UNetherCrownControlPPComponent::HandleCharacterHPModified(const float RemainHPRatio)
{
	SetLowHealthCameraPostProcessActive(RemainHPRatio < LowHealthPostProcessThreshold);
}

void UNetherCrownControlPPComponent::SetLowHealthCameraPostProcessActive(const bool bInActive)
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->IsLocallyControlled())
	{
		return;
	}

	UCameraComponent* HandledCameraComponent{ HandledCameraComponentWeak.Get() };
	if (!ensureAlways(IsValid(HandledCameraComponent)))
	{
		return;
	}

	HandledCameraComponent->PostProcessBlendWeight = bInActive ? 1.f : 0.f;
}

void UNetherCrownControlPPComponent::RestorePostProcessAfterCrowdControlFlash()
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->IsLocallyControlled())
	{
		return;
	}

	if (!IsValid(BoundPlayerStatComponent))
	{
		SetLowHealthCameraPostProcessActive(false);
		return;
	}

	const FNetherCrownPlayerStat& PlayerStatData{ BoundPlayerStatComponent->GetPlayerStatData() };
	if (PlayerStatData.CharacterMaxHP <= 0.f)
	{
		SetLowHealthCameraPostProcessActive(false);
		return;
	}

	HandleCharacterHPModified(PlayerStatData.CharacterHP / PlayerStatData.CharacterMaxHP);
}

void UNetherCrownControlPPComponent::StartClearPostProcessTimer(const float Duration)
{
	if (!ensureAlways(IsValid(CachedCharacter)) || !CachedCharacter->IsLocallyControlled())
	{
		return;
	}

	const UWorld* World{ GetWorld() };
	check(World);

	World->GetTimerManager().ClearTimer(TimerHandle_ClearPostProcess);
	World->GetTimerManager().SetTimer(TimerHandle_ClearPostProcess, this, &UNetherCrownControlPPComponent::StartSetPostProcessBlendEndTimeline, Duration, false);
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

	FPostProcessSettings* DefaultProcessSettings{ PostProcessCosmeticData.PostProcessSettingsMap.Find(ENetherCrownPPType::Default) };
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

void UNetherCrownControlPPComponent::SetPostProcessBlendStartByFloatTimeline(const float FloatCurveValue)
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

void UNetherCrownControlPPComponent::SetPostProcessBlendEndByFloatTimeline(const float FloatCurveValue)
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
