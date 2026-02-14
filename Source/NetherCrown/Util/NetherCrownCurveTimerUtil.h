// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FNetherCrownCurveTimerData
{
public:
	TObjectPtr<UObject> WorldContextObject{};
	UCurveFloat* Curve{};
	FTimerHandle* TimerHandle{};
	float* CurveElapsedTime{};
	float CurveElapsedTimeOffset{ 0.f };
	TFunction<void()> CallBack{};
};

class FNetherCrownCurveTimerUtil
{
public:
	static void BeginTimerByFloatCurve(const FNetherCrownCurveTimerData& CurveTimerData);
};
