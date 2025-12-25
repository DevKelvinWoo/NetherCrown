// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetherCrownSkillComponent.generated.h"

UENUM()
enum class EMyEnum : uint8
{
	QSkill,
	WSkill,
	ESkill,
	RSkill,
	ShiftSkill
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETHERCROWN_API UNetherCrownSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetherCrownSkillComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	//SkillUObject 클래스와 이 클래스를 이용해서 NewObject를 하는 함수
	//enum class에 맞춰서 해당 skill을 active하는 함수
	//Skill UObject parent class 생성
};
