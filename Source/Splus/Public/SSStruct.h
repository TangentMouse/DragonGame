// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StructUtils/UserDefinedStruct.h"
#include "UObject/StructOnScope.h"
#include "Engine/Texture2D.h"
#include "Engine/StaticMesh.h"
#include "Engine/Datatable.h"
#include "SSStruct.generated.h"

//背包物品类
UENUM(BlueprintType)
enum class RareLevel : uint8
{
	R1,    // ※
	R2,    // ※※
	R3,    // ※※※
	R4,		//※※※※
	R5		//※※※※※
};
UENUM(BlueprintType)
enum class ItemType : uint8
{
	A1,    // ※
	A2,    // ※※
	A3,    // ※※※
	A4,		//※※※※
	A5		//※※※※※
};
USTRUCT(BlueprintType)
struct FBData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Num;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Canduidie;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DuidieNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> ATextureforUI;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	RareLevel Rare;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ItemType Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UStaticMesh> StMesh; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UMaterialInterface> MeshTexture; 

	//TrashVoxel数据
	
};


USTRUCT(BlueprintType)
struct FDragonData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	//选择的actor

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DragonBaseNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HPIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DefenseIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpeedIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32>MOVE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32>Element;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isShanguang;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Tixing;

	//当前保存的数值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float De;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UpExpNeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NowHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32>ElementPotential;
	//哈哈哈哈这个东西是龙的名字(自定义的）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DnameByUser;
};

//接下来我会定义一个招式的变量，并且在每次开始运行后后载入代码中编辑的硬编码数据
USTRUCT()
struct FAllMOVEdata : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MOVE")
	int32 MElement;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MOVE")
	int32 MoveAT;
};


//下面这个变量是记录等级和学会招式，进化时候可能的进化路线
//1.等级和学习技能
USTRUCT()
struct FStudyMove : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MOVE")
	int32 Level;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MOVE")
	int32 LearnMove;
};
//等级学习技能和进化合集
USTRUCT(BlueprintType)
struct FEvolutionANDmove : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MOVE")
	FString Dname;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MOVE")
	TArray<FStudyMove>MOVEtable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MOVE")
	TArray<int32>EVOLUTION;
};



//包含TMap的数据结构(存储地图信息用）
USTRUCT(BlueprintType)
struct FSSMap
{
	GENERATED_USTRUCT_BODY()
public:
	TMap<int32,float >SingleBlockMap;
};


//针对不同物质的结构体(因为我是基础的FTableRowBase)
USTRUCT(BlueprintType)
struct FSSItem : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	//指向材质的软指针
	UPROPERTY(EditAnywhere, Category = "M")
	TSoftObjectPtr<UMaterialInterface> SoftMaterialPtr;
	//真硬度
	UPROPERTY(EditAnywhere, Category = "M")
	float yingdu;
	//孩子们这是掉落物
	UPROPERTY(EditAnywhere, Category = "M")
	TArray<FVector> diaoluowu;

};