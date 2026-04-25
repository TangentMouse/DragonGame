// Fill out your copyright notice in the Description page of Project Settings.


#include "CacheCharacter.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
// Sets default values
ACacheCharacter::ACacheCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//获取在线子系统
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		OnlineSubsystem->GetSessionInterface();
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage
			(
				-1,
				15.f,
				FColor::Blue,
				FString::Printf(TEXT("Found subsystem %s"), *OnlineSubsystem->GetSubsystemName().ToString())
			)
				;
		}
	}

}

// Called when the game starts or when spawned
void ACacheCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}
//
void ACacheCharacter::OpenTheMap()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel("/Game/MAP/1?Listen");
	}
}

void ACacheCharacter::CallOpenLevel(const FString& Address)
{
	UGameplayStatics::OpenLevel(this, *Address);
}

void ACacheCharacter::CallClientTravel(const FString& Address)
{
	// 修正变量名拼写错误
	APlayerController* PlayerController = GetGameInstance() ? GetGameInstance()->GetFirstLocalPlayerController() : nullptr;
	
	// 正确检查指针有效性
	if (PlayerController)
	{
		PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	}
}
//
void ACacheCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
