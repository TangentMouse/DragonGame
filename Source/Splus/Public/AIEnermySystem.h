// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIEnermySystem.generated.h"

USTRUCT(BlueprintType)
struct FCEnermy
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	AActor* EM;
	float Chouhenzhi;

};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPLUS_API UAIEnermySystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAIEnermySystem();

	// 优先攻击的敌人（蓝图可读写）
	UPROPERTY(BlueprintReadWrite, Category = "Enemy Targeting")
	AActor* TargetEnermy;

	// 所有被标记的敌人（蓝图只读，避免蓝图随意修改数组）
	UPROPERTY(BlueprintReadOnly, Category = "Enemy Targeting")
	TArray<FCEnermy> EArray;
protected:
	// Called when the game starts
	void BeginPlay() override;

public:	
	// Called every frame
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:

	//我最喜欢的敌人系统的函数
	UFUNCTION(BlueprintCallable)
	void AddEnermy(AActor* Enermy, float Damage);
	UFUNCTION(BlueprintCallable)
	//寻找输入目标是否是仇恨对象
	bool FindISEnermy(AActor* Enermy);


	UFUNCTION(BlueprintCallable)
	void TICKcheck();


		
};
