// Fill out your copyright notice in the Description page of Project Settings.


#include "NetherCrownKnightAnimInstance.h"

#include "NetherCrown/NetherCrown.h"
#include "NetherCrown/Character/NetherCrownCharacter.h"

void UNetherCrownKnightAnimInstance::AnimNotify_HitSkyFallSlashSkill()
{
	if (!IsValid(CachedOwningCharacter))
	{
		UE_LOG(LogNetherCrown, Error, TEXT("CachedOwningCharacter is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	OnHitSkyFallSlashSkill.Broadcast();
}

void UNetherCrownKnightAnimInstance::AnimNotify_HitFrozenTempestSkill()
{
	if (!IsValid(CachedOwningCharacter))
	{
		UE_LOG(LogNetherCrown, Error, TEXT("CachedOwningCharacter is not valid %hs"), __FUNCTION__);
		return;
	}

	if (!CachedOwningCharacter->HasAuthority())
	{
		return;
	}

	OnHitFrozenTempestSkill.Broadcast();
}
