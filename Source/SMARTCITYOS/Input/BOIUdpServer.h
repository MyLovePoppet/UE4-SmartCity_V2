// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UdpServerUtilities.h"
#include "GameFramework/Actor.h"
#include "BOIUdpServer.generated.h"

class FBoiUdpReceiver : public FRunnable
{
    FThreadSafeCounter StopTaskCounter; //用于多线程间的判断交互，volatile int32 Counter;
    //int32 FindNextPrimeNumber();


    FSocket* Socket;
    FUdpSocketReceiver* UdpSocketReceiver;
    
public:

    // Begin FRunnable interface.
    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;
    // End FRunnable interface
    
    void StartUDPReceiver(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort,
                              bool& success); // 接收器初始化  接收信息前
    
    void DataRecv(FString& str, bool& success);
protected:
    virtual void Handle(const TSharedPtr<FJsonObject>& JsonObject);
};


UCLASS()
class SMARTCITYOS_API ABOIUdpServer : public AActor
{
    GENERATED_BODY()
    TSharedPtr<FRunnable>DragUdpRunnable;
    TSharedPtr<FRunnableThread>DragUdpThread;
    
public:
    // Sets default values for this actor's properties
    ABOIUdpServer();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
};
