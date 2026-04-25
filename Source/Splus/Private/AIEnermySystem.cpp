// Fill out your copyright notice in the Description page of Project Settings.

#include "AIEnermySystem.h"

// Sets default values for this component's properties
UAIEnermySystem::UAIEnermySystem()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UAIEnermySystem::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UAIEnermySystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TICKcheck();
}

void UAIEnermySystem::AddEnermy(AActor* Enermy, float Damage)
{
	if (!IsValid(Enermy)) return; // 无效敌人直接返回

	bool bFound = false;
	// 遍历查找是否已存在该敌人
	for (int32 i = 0; i < EArray.Num(); i++)
	{
		if (EArray[i].EM == Enermy)
		{
			// 已存在则累加仇恨值
			EArray[i].Chouhenzhi += Damage;
			bFound = true;
			break; // 找到后退出循环
		}
	}

	// 如果不存在，则添加新敌人
	if (!bFound)
	{
		FCEnermy NewEnemy;
		NewEnemy.EM = Enermy;
		NewEnemy.Chouhenzhi = Damage;
		EArray.Add(NewEnemy);
	}
}

void UAIEnermySystem::TICKcheck()
{
	// 1. 衰减仇恨值并移除仇恨值过低的敌人（从后往前遍历避免索引错乱）
	for (int32 i = EArray.Num() - 1; i >= 0; i--)
	{
		// 检查敌人是否有效（防止引用已销毁的Actor）
		if (!IsValid(EArray[i].EM))
		{
			EArray.RemoveAt(i);
			continue;
		}

		// 衰减仇恨值
		EArray[i].Chouhenzhi /= 1.03f;

		// 移除仇恨值过低的敌人
		if (EArray[i].Chouhenzhi < 1.0f)
		{
			EArray.RemoveAt(i);
		}
	}

	// 2. 选择最大仇恨值的敌人作为目标
	if (EArray.Num() > 0)
	{
		float MaxHate = 0.0f;
		AActor* NewTarget = nullptr;

		// 一次遍历找到最大仇恨值的敌人
		for (const FCEnermy& Enemy : EArray)
		{
			if (IsValid(Enemy.EM) && Enemy.Chouhenzhi > MaxHate)
			{
				MaxHate = Enemy.Chouhenzhi;
				NewTarget = Enemy.EM;
			}
		}

		TargetEnermy = NewTarget;
	}
	else
	{
		// 没有敌人时清空目标
		TargetEnermy = nullptr;
	}
}


bool UAIEnermySystem::FindISEnermy(AActor* Enermy)
{
	if (!IsValid(Enermy)) return false;

	for (const FCEnermy& Enemy : EArray)
	{
		if (Enemy.EM == Enermy)
		{
			return true;
		}
	}
	return false;
}
