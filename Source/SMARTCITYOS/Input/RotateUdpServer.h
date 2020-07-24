// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UdpServerBase.h"
#include "RotateUdpServer.generated.h"

/**
 * 
 */
UCLASS()
class SMARTCITYOS_API ARotateUdpServer : public AUdpServerBase
{
    GENERATED_BODY()
    float CurrentDegree = 0.f;
    //创建可复用的Socket和RemoteAddress，方便发送，就不用后面发送一个创建一个了
    FSocket* OurReuseAbleSocket;
    TSharedPtr<FInternetAddr> OurReuseAbleRemoteAddress;
    //初始化，返回是否成功
    bool InitOurSocket(const FString& TheIP = TEXT("192.168.50.247"),
                       const uint32 ThePort = EOperationPort::SEND_BACK_PORT,
                       const FString& YourChosenSocketName = TEXT("SendBackDegree"));

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Handle(const TSharedPtr<FJsonObject>& JsonObject) override;
};
