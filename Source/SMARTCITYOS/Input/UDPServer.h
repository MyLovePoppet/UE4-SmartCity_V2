// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Networking/Public/Networking.h"

#include "UDPServer.generated.h"

UENUM()
enum class EOperationType:uint8
{
    MOVE,
    SCALE,
    ROTATE,
    MOVE_START
};

UCLASS()
class SMARTCITYOS_API AUDPServer : public AActor
{
    GENERATED_BODY()

    FSocket* Socket;
    FUdpSocketReceiver* UdpSocketReceiver;
    public:
    static FVector2D PhoneScreenSize;
    static FVector2D PCScreenSize;
public:
    // Sets default values for this actor's properties
    AUDPServer();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    void StartUDPReceiver(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort,
                          bool& success); // 接收器初始化  接收信息前

    void DataRecv(FString& str, bool& success);

    void Handle(const TSharedPtr<FJsonObject>& JsonObject);

    static bool GetJsonObjectFromJsonFString(const FString& _jsonFString, TSharedPtr<FJsonObject>& _jsonObject);

    static EOperationType ToEnumType(const FString& Str);
    static FVector2D ToPCLocation(FVector2D PhoneLocation);
};

inline EOperationType AUDPServer::ToEnumType(const FString& Str)
{
    if (Str.ToLower() == "move")
    {
        return EOperationType::MOVE;
    }
    if (Str.ToLower() == "scale")
    {
        return EOperationType::SCALE;
    }
    if (Str.ToLower() == "rotate")
    {
        return EOperationType::ROTATE;
    }
    if(Str.ToLower()=="movestart")
    {
        return EOperationType::MOVE_START;
    }
    //this will be no longer called
    return EOperationType::MOVE;
}
