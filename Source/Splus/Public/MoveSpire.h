// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MoveSpire.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPLUS_API UMoveSpire : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMoveSpire();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};


//属性	效果说明(待更新内容)
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
