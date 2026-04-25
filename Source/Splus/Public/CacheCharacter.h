#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OnlineSubsystem.h"
#include "CacheCharacter.generated.h"

UCLASS()
class SPLUS_API ACacheCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // 构造函数（公共成员，先声明）
    ACacheCharacter();

    // 每帧更新（公共成员）
    virtual void Tick(float DeltaTime) override;

protected:
    // 游戏开始时调用（受保护成员）
    virtual void BeginPlay() override;

    // （可添加其他受保护成员，如变量、函数）

private:
    // （若有私有成员，在此声明）
public:

   UFUNCTION(BlueprintCallable)
    void OpenTheMap();
   UFUNCTION(BlueprintCallable)
    void CallOpenLevel(const FString& Address);
   UFUNCTION(BlueprintCallable)
    void CallClientTravel(const FString& Address);
//在线系统部分

public:
    
    TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe> OnlineSessionInterface;
}; 