// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent/Public/ProceduralMeshComponent.h"
#include "TVoxel.generated.h"
class ATLoadWorld;
UCLASS()
class SPLUS_API ATVoxel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATVoxel();
	//传入体素参数
	//纯CPP
	//加载蓝图数据表
	UDataTable* LoadSSItem();
	UDataTable* LoadMaptable;









	// 声明 Procedural Mesh 组件
	UPROPERTY(VisibleAnywhere, Category = "Procedural Mesh")
	UProceduralMeshComponent* ProceduralMesh;
	UFUNCTION()
	void GenerateTVoxelmin(TArray<float> voxels);

	
	UFUNCTION()
	float GetOffset(float v1, float v2);
	
public:
	//读取到外部的TLoadWorld的哈希表里面去刷新整个这个
	UFUNCTION(BlueprintCallable)
	TArray<float> ReadTandRenew(FVector Local, float banjin, bool wa);



	//基岩
	  // 基岩组件（静态Cube）
	UPROPERTY(VisibleAnywhere, Category = "Bedrock")
	UStaticMeshComponent* BedrockCube;
};
