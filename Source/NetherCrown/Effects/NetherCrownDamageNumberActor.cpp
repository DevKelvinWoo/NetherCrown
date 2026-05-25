// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownDamageNumberActor.h"

#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "NetherCrown/Widgets/NetherCrownDamageNumberWidgetView.h"

ANetherCrownDamageNumberActor::ANetherCrownDamageNumberActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRootComponent"));
	SetRootComponent(SceneRootComponent);

	DamageNumberWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageNumberWidgetComponent"));
	DamageNumberWidgetComponent->SetupAttachment(RootComponent);
	DamageNumberWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	DamageNumberWidgetComponent->SetDrawAtDesiredSize(true);
	DamageNumberWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ANetherCrownDamageNumberActor::InitDamageNumber(const int32 DamageAmount, const bool bIsCriticalDamage)
{
	if (!ensureAlways(IsValid(DamageNumberWidgetComponent)))
	{
		SetLifeSpan(FallbackLifeSpan);
		return;
	}

	DamageNumberWidgetComponent->InitWidget();

	UNetherCrownDamageNumberWidgetView* DamageNumberWidgetView{ Cast<UNetherCrownDamageNumberWidgetView>(DamageNumberWidgetComponent->GetWidget()) };
	if (!ensureAlways(IsValid(DamageNumberWidgetView)))
	{
		SetLifeSpan(FallbackLifeSpan);
		return;
	}

	DamageNumberWidgetView->GetOnDamageNumberAnimationFinished().AddUObject(this, &ThisClass::HandleDamageNumberAnimationFinished);
	DamageNumberWidgetView->InitDamageNumber(DamageAmount, bIsCriticalDamage);
	DamageNumberWidgetView->PlayDamageNumberAnimation();

	SetLifeSpan(FallbackLifeSpan);
}

void ANetherCrownDamageNumberActor::HandleDamageNumberAnimationFinished()
{
	Destroy();
}
