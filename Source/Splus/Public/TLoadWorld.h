// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SSStruct.h"
#include "TLoadWorld.generated.h"
class ATVoxel;




UCLASS()
class SPLUS_API ATLoadWorld : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATLoadWorld();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY()
	TArray<AActor*> OwnedActors;
	//标记已生成位置的tmap(生成时调用）
	UPROPERTY()
	TMap<FString, ATVoxel*> TVtmap;


	//存档和临时记录需要用的这个TMAP(神秘嵌套Fmap，生成时辅助调用)
	UPROPERTY()
	TMap<FString,FSSMap>ReLoadMap;


	UFUNCTION(BlueprintCallable)
	void SpawnVoxel(FVector Local);

	UFUNCTION()
	void tickFUN();


	//测试
	//分配gei'w
	UPROPERTY(VisibleAnywhere, Category = "Test")
	TArray<float>TV;
	//隔壁的函数
	UFUNCTION()
	void SDFSpawn(FVector loc);


public:
	// 柏林噪声核心函数
	float PerlinNoise2D(float x, float y);
	float Fade(float t);
	float Lerp(float t, float a, float b);
	float Grad(int32 hash, float x, float y);

	// 置换表（内置，避免外部依赖）
	static const int32 Permutation[256];
	static const int32 PermutationExtended[512];


public:
	//挖掘函数
	UFUNCTION()
	TArray<float> DigHole(FVector Center, float Radius);
	//放置函数
	

	// 9×9×129 顶点维度（体素维度8×8×128）
	const int32 VertexDimX = 9;          // X轴顶点数（9）
	const int32 VertexDimY = 9;          // Y轴顶点数（9）
	const int32 VertexDimZ = 129;        // Z轴顶点数（129，原65→提升）
	const float BlockSizeX = 800.0f;     // X轴区块尺寸（8×100=800，不变）
	const float BlockSizeY = 800.0f;     // Y轴区块尺寸（8×100=800，不变）
	const float BlockSizeZ = 12800.0f;   // Z轴区块尺寸（128×100=12800，原6400→翻倍）
	const float VoxelSize = 100.0f;      // 体素大小（100单位，不变）






	// 在头文件中添加可配置变量（建议在编辑器中暴露以便调整）

};
