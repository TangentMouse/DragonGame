// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SSStruct.h"
#include "GameSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class SPLUS_API UGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UGameSubsystem();
	//±‰¡ø1

	TArray<FAllMOVEdata> MTb;

	void LoadMOVE();
};
