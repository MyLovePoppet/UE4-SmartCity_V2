// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UdpServerUtilities.h"
#include "UdpServerBase.generated.h"

UCLASS()
class SMARTCITYOS_API AUdpServerBase : public AActor
{
    GENERATED_BODY()
protected:
    FSocket* Socket;
    FUdpSocketReceiver* UdpSocketReceiver;


    // Sets default values for this actor's properties
    AUdpServerBase();

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    void StartUDPReceiver(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort,
                          bool& success); // 接收器初始化  接收信息前
    
    void DataRecv(FString& str, bool& success);
protected:
    virtual void Handle(const TSharedPtr<FJsonObject>& JsonObject);
};
