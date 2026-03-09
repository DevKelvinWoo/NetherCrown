// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownEnemyStatComponent.h"

UNetherCrownEnemyStatComponent::UNetherCrownEnemyStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UNetherCrownEnemyStatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UNetherCrownEnemyStatComponent::SetEnemyHp(int32 InHp)
{
	EnemyStatData.EnemyHP = InHp;
}
