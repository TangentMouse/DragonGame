// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SSStruct.h"
#include "CInventory.generated.h"
//数据结构部分
// add函数剩余结构体	
USTRUCT(BlueprintType)
struct FAddreturn
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LessFigure;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isfull;
};
//按照索引删除索引的东西和对应数量（数组对应，请不要索引错误了
USTRUCT(BlueprintType)
struct FDeleteItem
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBData NeedITEM;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemAmount;
};
//孩子们你们知道这个结构体是用来干嘛的
USTRUCT(BlueprintType)
struct FDeleteItemArray
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDeleteItem> DeleteItemData;
};
//不要动

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPLUS_API UCInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCInventory();
	//变量
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CSlot = 16;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Maxduidie = 99;
	//数组
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FBData> ItemData;
	//库存操作函数
	//0.刷新数组的函数
	UFUNCTION(BlueprintCallable)
	void ReSetArray();
	//1.添加的函数
	UFUNCTION(BlueprintCallable)
	FAddreturn AddStructToArray(FBData Playerbackpack);
	//2.按照索引移除单个格子内容的函数
	UFUNCTION(BlueprintCallable)
	FBData RemoveSingleSlot(int32 SlotNum);
	//3.按照指定的物品与数量去移除
	//3-1检索函数
	UFUNCTION(BlueprintCallable)
	bool isAllEnough(FDeleteItemArray NeedItem);
	//3-2删除函数（按照索引和数量）
	UFUNCTION(BlueprintCallable)
	void deleteItem(FDeleteItemArray DeleteItem);
	//3-3ui附加相同物品
	UFUNCTION(BlueprintCallable)
	FBData CalculateChangeItem(FDeleteItem IntItemDatas);

};
