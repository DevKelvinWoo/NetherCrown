// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownPortal.h"

#include "Components/SceneCaptureComponentCube.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/TextureRenderTargetCube.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NetherCrown/Data/NetherCrownInteractData.h"
#include "NetherCrown/GameMode/NetherCrownGameMode.h"

ANetherCrownPortal::ANetherCrownPortal()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	bNetLoadOnClient = true;
}

void ANetherCrownPortal::BeginPlay()
{
	Super::BeginPlay();

	InitializePortalRenderTargets();
	CapturePortalScene();

	if (UWorld* World{ GetWorld() })
	{
		World->GetTimerManager().SetTimer(TimerHandle_CapturePortalScene, this, &ThisClass::CapturePortalScene, 0.35f, true);
	}
}

void ANetherCrownPortal::FinishInteract(ANetherCrownCharacter* InteractCharacter)
{
	Super::FinishInteract(InteractCharacter);

	if (HasAuthority())
	{
		TravelByLevelTag();
	}
}

void ANetherCrownPortal::InitializePortalRenderTargets()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	TArray<USceneCaptureComponentCube*> SceneCaptureComponents{};
	GetComponents<USceneCaptureComponentCube>(SceneCaptureComponents);
	if (SceneCaptureComponents.IsEmpty())
	{
		return;
	}

	RuntimePortalRenderTargets.Empty();

	for (USceneCaptureComponentCube* SceneCaptureComponent : SceneCaptureComponents)
	{
		if (!IsValid(SceneCaptureComponent))
		{
			continue;
		}

		const UTextureRenderTargetCube* SourceTextureTarget{ SceneCaptureComponent->TextureTarget.Get() };
		const int32 SourceTextureSize{ IsValid(SourceTextureTarget) && SourceTextureTarget->SizeX > 0 ? SourceTextureTarget->SizeX : 512 };
		const int32 TextureSize{ FMath::Clamp(SourceTextureSize, 128, 512) };

		UTextureRenderTargetCube* RuntimeRenderTarget{ NewObject<UTextureRenderTargetCube>(this) };
		if (!ensureAlways(IsValid(RuntimeRenderTarget)))
		{
			continue;
		}

		RuntimeRenderTarget->ClearColor = FLinearColor::Black;
		RuntimeRenderTarget->InitAutoFormat(TextureSize);
		RuntimeRenderTarget->UpdateResourceImmediate(true);

		SceneCaptureComponent->TextureTarget = RuntimeRenderTarget;
		SceneCaptureComponent->bCaptureEveryFrame = false;
		SceneCaptureComponent->bCaptureOnMovement = false;

		RuntimePortalRenderTargets.Add(RuntimeRenderTarget);
	}

	if (RuntimePortalRenderTargets.IsEmpty())
	{
		return;
	}

	UTextureRenderTargetCube* PortalTextureTarget{ RuntimePortalRenderTargets[0] };

	TArray<UStaticMeshComponent*> StaticMeshComponents{};
	GetComponents<UStaticMeshComponent>(StaticMeshComponents);

	for (UStaticMeshComponent* StaticMeshComponent : StaticMeshComponents)
	{
		if (!IsValid(StaticMeshComponent))
		{
			continue;
		}

		const int32 MaterialNum{ StaticMeshComponent->GetNumMaterials() };
		for (int32 MaterialIndex{ 0 }; MaterialIndex < MaterialNum; ++MaterialIndex)
		{
			UMaterialInstanceDynamic* DynamicMaterial{ StaticMeshComponent->CreateDynamicMaterialInstance(MaterialIndex) };
			if (!IsValid(DynamicMaterial))
			{
				continue;
			}

			DynamicMaterial->SetTextureParameterValue(TEXT("CubeMapTexture"), PortalTextureTarget);
		}
	}
}

void ANetherCrownPortal::CapturePortalScene()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	TArray<USceneCaptureComponentCube*> SceneCaptureComponents{};
	GetComponents<USceneCaptureComponentCube>(SceneCaptureComponents);

	for (USceneCaptureComponentCube* SceneCaptureComponent : SceneCaptureComponents)
	{
		if (!IsValid(SceneCaptureComponent) || !IsValid(SceneCaptureComponent->TextureTarget.Get()))
		{
			continue;
		}

		SceneCaptureComponent->CaptureScene();
	}

	--RemainPortalCaptureRetryCount;
	if (RemainPortalCaptureRetryCount <= 0)
	{
		for (USceneCaptureComponentCube* SceneCaptureComponent : SceneCaptureComponents)
		{
			if (!IsValid(SceneCaptureComponent))
			{
				continue;
			}

			SceneCaptureComponent->bCaptureEveryFrame = false;
			SceneCaptureComponent->bCaptureOnMovement = false;
		}

		if (UWorld* World{ GetWorld() })
		{
			World->GetTimerManager().ClearTimer(TimerHandle_CapturePortalScene);
		}
	}
}

void ANetherCrownPortal::TravelByLevelTag()
{
	if (!HasAuthority())
	{
		return;
	}

	const UWorld* World{ GetWorld() };
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}

	ANetherCrownGameMode* NetherCrownGameMode{ World->GetAuthGameMode<ANetherCrownGameMode>() };
	if (!ensureAlways(IsValid(NetherCrownGameMode)))
	{
		return;
	}

	const UNetherCrownPortalDataAsset* PortalDataAsset{ Cast<UNetherCrownPortalDataAsset>(GetCachedInteractDataAsset()) };
	if (!ensureAlways(IsValid(PortalDataAsset)))
	{
		return;
	}

	NetherCrownGameMode->TravelLevelByTag(PortalDataAsset->GetLevelTag());
}
