#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SSStruct.h"
#include "DSaveGame.generated.h"


/**
 * 
 */
UCLASS()
class SPLUS_API UDSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	// 存档名称（用于区分不同存档）
	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	FString SaveSlotName;
	//存档地图情况，，F2D第一个值是坐标值，第二个是数据(同一String最多1000个）
	
};
