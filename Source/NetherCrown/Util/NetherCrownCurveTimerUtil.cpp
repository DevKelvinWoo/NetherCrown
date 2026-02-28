// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownCurveTimerUtil.h"

#include "NetherCrown/NetherCrown.h"

void FNetherCrownCurveTimerUtil::ExecuteLoopTimerCallbackByCurve(const FNetherCrownCurveTimerData& CurveTimerData)
{
	const TObjectPtr<UObject> WorldContextObject{ CurveTimerData.WorldContextObject };
	if (!ensureAlways(IsValid(WorldContextObject)))
	{
		return;
	}

	const UWorld* World{ WorldContextObject->GetWorld() };
	check(World);

	FTimerHandle* TimerHandle{ CurveTimerData.TimerHandle };
	if (!ensureAlways(TimerHandle))
	{
		return;
	}

	const UCurveBase* Curve{ CurveTimerData.Curve };
	if (!ensureAlways(IsValid(Curve)))
	{
		World->GetTimerManager().ClearTimer(*TimerHandle);
		return;
	}

	float* CurveElapsedTime{ CurveTimerData.CurveElapsedTime };
	if (!CurveElapsedTime)
	{
		UE_LOG(LogNetherCrown, Warning, TEXT("Curve Elasped Time pointer is nullptr"));
		return;
	}

	*CurveElapsedTime += CurveTimerData.CurveElapsedTimeOffset;

	float Min{};
	float Max{};
	Curve->GetTimeRange(Min, Max);

	if (*CurveElapsedTime > Max)
	{
		TFunction<void()> ClearCallBack{ CurveTimerData.ClearCallBack };
		if (ClearCallBack.IsSet())
		{
			CurveTimerData.ClearCallBack();
		}

		World->GetTimerManager().ClearTimer(*TimerHandle);
	}

	TFunction<void()> CallBack{ CurveTimerData.CallBack };
	if (CallBack.IsSet())
	{
		CurveTimerData.CallBack();
	}
}
