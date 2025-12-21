// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyStatComponent.h"

UNetherCrownEnemyStatComponent::UNetherCrownEnemyStatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UNetherCrownEnemyStatComponent::SetEnemyHp(int32 InHp)
{
	EnemyStatData.EnemyHP = InHp;
}

void UNetherCrownEnemyStatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UNetherCrownEnemyStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
