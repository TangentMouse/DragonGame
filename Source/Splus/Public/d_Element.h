// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
//获取subsystem
#include "GameSubsystem.h" 
#include "CoreMinimal.h"
#include "SSStruct.h"
#include "Engine/GameInstance.h" 
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Dragon_Inventory.h"
#include "d_Element.generated.h"



//分割线

USTRUCT(BlueprintType)
struct FSelfData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Element")
	TArray<int32> SelfElement;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Element")
	int32 Attack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Element")
	int32 MaxHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Element")
	int32 Defense;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Element")
	int32 Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Element")
	int32 Level;

};
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPLUS_API Ud_Element : public UActorComponent
{
	GENERATED_BODY()
//元素组件
public:	
	// Sets default values for this component's properties
	Ud_Element();
	//非蓝图变量


	UGameSubsystem* GameSubsystem;

	



	//蓝图变量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Element")
	FSelfData SingleDragonD;
	//从蓝图获取的变量
	

	//难如绷buff系统一个FV做的(Z轴是干什么的只有天知道)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuffSystem")
	TArray<FVector> BaseBuff;
	int32 HowMuchBuff = 10;

public:
	//非蓝图函数
	//1.获取子进程
	void getSubsystem();
	//2.加载蓝图数据表
	UDataTable* LoadEvolutionDatabase();
	


	//蓝图可获取函数//这个float数组是给野怪用的
	UFUNCTION(BlueprintCallable)
	TArray<float> ChushihuaData(FDragonData data);
	UFUNCTION(BlueprintCallable)
	int32 MoveAppendElement(int32 MoveElement, int32 actorElement);
	UFUNCTION(BlueprintCallable, Category = "Damage")
	int32 toRealDamage(int32 userlevel,
		int32 whichMOVE,
		float attack,
		float defense,
		TArray<int32> whichElement,
		TArray<int32> SelfElement,
		TArray<FVector> UserBuff);
	//不好意思，也怪跟路边一条一样，就不在c++里面专门写函数改数值了
	
	//仅计算升级所需
	UFUNCTION(BlueprintCallable)
	float CalLONLY(int32 LEVEL);

	//载入等级学习技能
	UFUNCTION(BlueprintCallable)
	TArray<int32> IntStudyMove(int32 Level, int32 DragonFromDatatable);
	
	//重载技能
	UFUNCTION(BlueprintCallable)
	TArray<int32> ReSetMove(TArray<int32> intMOVE, int32 Level, int32 DragonFromDatatable);




	UFUNCTION(BlueprintCallable)
	void ADDBuff(int32 WhichBuff, FVector LevelAndDelay);

	UFUNCTION(BlueprintCallable)
	float GBF(int32 whichIndex, TArray<FVector> BuffArray);


	UFUNCTION(BlueprintCallable)
	void RemoveBuff();


};




//属性	效果说明
//金属	物理类型攻击范围扩大 35°
//离子	攻击可造成 “电离” 减益效果
//数据	伤害的 20 % 可无视敌人任何抵抗
//声音	能量类弹幕可在墙壁或敌人间反弹，回合内造成第二次伤害（为第一次的 50 % ）
//有机	不攻击时自动回复 HP
//自然	攻击可回复自身伤害 2 % 的血量
//原始	原始技能伤害变为 1.35 倍
//时间	每 60 秒获得一次无敌状态
//液体	受伤时减伤 25 %
//冻结	攻击命中敌人造成 “冻结” 减益（减速效果）
//蒸汽	水属性招式命中敌人后，再造成一段原伤害 25 % 的 “蒸发伤害”
//氧化	攻击命中敌人造成 “燃烧” 减益
//能量	所有能量（驱动的、有机核能、内能、光能、核能等）攻速 ×1.4
//光明	特殊能量类技能范围扩大 35 %
//辐射	能量类技能持续残留 2.5 秒
//天体	发射弹幕（射击类）不再直线发射，而是做离心运动
//黑暗	免疫所有减益效果
//空间	被技能命中时有 10 % 概率免伤
//混乱	攻击后造成伤害在 0.9~1.4 倍之间随机波动
