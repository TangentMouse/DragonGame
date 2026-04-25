// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SSStruct.h"
#include "Dragon_Inventory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPLUS_API UDragon_Inventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDragon_Inventory();
	//队伍里面龙的数量
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DSlot = 99;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDragonData> DData;



	//队伍操作函数(蓝图用的）
	//0.初始化函数
	UFUNCTION(BlueprintCallable)
	void DReSetArray();
	//1.添加的函数
	UFUNCTION(BlueprintCallable)
	bool AddDragonToArray(FDragonData PlayerDragon);
	//2.按照索引移除单个格子内容的函数
	UFUNCTION(BlueprintCallable)
	bool RemoveDragonFromArray(int32 SlotNum);
	//3.按照龙的base编号去索引到库存里面的龙
	UFUNCTION(BlueprintCallable)
	TArray<int32> GetDragonFromNum(int32 DragonNum);
	//4.交换龙的顺序
	UFUNCTION(BlueprintCallable)
	void EXchangeDragon(int32 num1,int32 num2);

	//计算数值系统。纯求返回值
	UFUNCTION(BlueprintCallable)
	TArray<float> ChushihuaData(FDragonData data);
	//计算升级(这个小小二维数组里面第一个值是
	UFUNCTION(BlueprintCallable)
	void CalcuUpEXPandLEVEL(int32 Num,float EXP);
	UFUNCTION(BlueprintCallable)
	float CalcuLevelONLY(int32 LEVEL);
	//通过计算公式造成真伤(针对玩家数据数组)
	UFUNCTION(BlueprintCallable)
	void RealDamage(int32 Damage, int32 INum);


	//技能替换功能
	UFUNCTION(BlueprintCallable)
	void MoveChange(int32 DNUM, int32 M1, int32 M2);




	//元素进化功能
	//元素获取
	UFUNCTION(BlueprintCallable)
	void AddElementPotential(int32 Enum, int32 Dnum);

	//还未完善的检测元素进化功能
	UFUNCTION(BlueprintCallable)
	int32 ELementUP(int32 Dnum);
};
