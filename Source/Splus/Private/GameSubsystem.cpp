// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSubsystem.h"
#include "SSStruct.h"


UGameSubsystem::UGameSubsystem()
{
	LoadMOVE();
}

void UGameSubsystem::LoadMOVE()
{
	//初始化技能表格
	MTb.Init(FAllMOVEdata{}, 100);
	//技能1 原始属性 威力4
	MTb[1].MElement = 7;
	MTb[1].MoveAT = 4;
	//技能2 原始属性 威力5
	MTb[2].MElement = 7;
	MTb[2].MoveAT = 5;
	//技能3 氧化属性 威力5
	MTb[3].MElement = 13;
	MTb[3].MoveAT = 5;
	//技能4 原始属性 威力6
	MTb[4].MElement = 7;
	MTb[4].MoveAT = 6;
	//技能5 原始属性 威力4
	MTb[5].MElement = 7;
	MTb[5].MoveAT = 4;
	//技能6 液体属性 威力4
	MTb[6].MElement = 9;
	MTb[6].MoveAT = 4;
	//技能7 金属属性 威力5
	MTb[7].MElement = 1;
	MTb[7].MoveAT = 5;
	//技能8 天体属性 威力1
	MTb[8].MElement = 17;
	MTb[8].MoveAT = 1;
	//技能9 氧化属性 威力1
	MTb[9].MElement = 7;
	MTb[9].MoveAT = 1;
	//技能10 天体属性 威力8
	MTb[10].MElement = 17;
	MTb[10].MoveAT = 8;
}