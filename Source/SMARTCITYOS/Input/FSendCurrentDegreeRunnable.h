// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UdpServerUtilities.h"

/**
 * 
 */
class SMARTCITYOS_API FSendCurrentDegreeRunnable : public FRunnable
{
    TQueue<FString> MessageToBeSend;
    //创建可复用的Socket和RemoteAddress，方便发送，就不用后面发送一个创建一个了
    FSocket* OurReuseAbleSocket;
    TSharedPtr<FInternetAddr> OurReuseAbleRemoteAddress;
    bool CurrentThreadIsStopped = false;
public:
    bool Init() override;
    //初始化，返回是否成功
    bool InitOurSocket(const FString& TheIP = TEXT("192.168.50.247"),
                       const uint32 ThePort = EOperationPort::SEND_BACK_PORT,
                       const FString& YourChosenSocketName = TEXT("SendBackDegree"));
    uint32 Run() override;
    /**
     * 消息入队列，自动消费
     * @param Message 入队列准备处理的数据
     */
    void Enqueue(const FString& Message);
    void Stop() override;
    void Exit() override;
};
