// Fill out your copyright notice in the Description page of Project Settings.

#include "NetherCrownSkillDashAttack.h"

#include "GameFramework/CharacterMovementComponent.h"

void UNetherCrownSkillDashAttack::PlaySkillCosmetics()
{
	Super::PlaySkillCosmetics();

	//DashOwnerCharacter();
}

void UNetherCrownSkillDashAttack::ExecuteSkillGameplay()
{
	Super::ExecuteSkillGameplay();

	//DashOwnerCharacter();
}

void UNetherCrownSkillDashAttack::DashOwnerCharacter() const
{
	ANetherCrownCharacter* SkillOwnerCharacter{ SkillOwnerCharacterWeak.Get() };
	if (!ensureAlways(IsValid(SkillOwnerCharacter)))
	{
		return;
	}

	if (SkillOwnerCharacter->GetCharacterMovement())
	{
		// 1. Root Motion Source 생성 (ConstantForce: 일정 속도로 밀어줌)
		TSharedPtr<FRootMotionSource_ConstantForce> ConstantForce = MakeShared<FRootMotionSource_ConstantForce>();

		// 2. 이름 설정 (디버깅용)
		ConstantForce->InstanceName = TEXT("Dash");

		// 3. 덮어쓰기 모드 (기존의 중력이나 움직임을 무시하려면 Override)
		ConstantForce->AccumulateMode = ERootMotionAccumulateMode::Override;

		// 4. 우선순위 (높을수록 다른 소스를 덮어씀)
		ConstantForce->Priority = 5;

		// 5. 힘과 방향 설정
		// 속도 = 거리 / 시간
		float Speed = 500.f / 0.25;

		FVector ForwardDir = SkillOwnerCharacter->GetActorForwardVector();
		ForwardDir.Z = 0.0f; // 바닥으로 꽂히지 않게 수평 유지
		ForwardDir.Normalize();
		ConstantForce->Force = ForwardDir * Speed;

		// 6. 지속 시간
		ConstantForce->Duration = 0.25;

		// 7. 끝났을 때 처리 (멈출 것인가, 관성을 유지할 것인가)
		// 여기서는 대쉬 후 즉시 정지하도록 설정
		ConstantForce->FinishVelocityParams.Mode = ERootMotionFinishVelocityMode::SetVelocity;
		ConstantForce->FinishVelocityParams.SetVelocity = FVector::ZeroVector;

		// 8. 적용
		SkillOwnerCharacter->GetCharacterMovement()->ApplyRootMotionSource(ConstantForce);
	}
}
