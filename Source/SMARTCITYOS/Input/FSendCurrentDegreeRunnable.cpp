// Fill out your copyright notice in the Description page of Project Settings.


#include "FSendCurrentDegreeRunnable.h"


bool FSendCurrentDegreeRunnable::Init()
{
    return InitOurSocket();
}

bool FSendCurrentDegreeRunnable::InitOurSocket(const FString& TheIP, const uint32 ThePort,
                                               const FString& YourChosenSocketName)
{
    OurReuseAbleRemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    bool bIsValid;
    OurReuseAbleRemoteAddress->SetIp(*TheIP, bIsValid);
    OurReuseAbleRemoteAddress->SetPort(ThePort);
    if (!bIsValid)
    {
        UE_LOG(LogTemp, Error, TEXT("UDP Sender>> IP address was not valid!"));
        return false;
    }
    OurReuseAbleSocket = FUdpSocketBuilder(*YourChosenSocketName)
                         .AsReusable()
                         .WithBroadcast() /////////////广播  
                         .WithSendBufferSize(1024);
    //Set Send Buffer Size  
    int32 SendSize = 1024;
    OurReuseAbleSocket->SetSendBufferSize(SendSize, SendSize);
    OurReuseAbleSocket->SetReceiveBufferSize(SendSize, SendSize);
    UE_LOG(LogTemp, Error, TEXT("UDP Sender>> UDP Sender start OK!"));
    return bIsValid;
}

uint32 FSendCurrentDegreeRunnable::Run()
{
    while (!CurrentThreadIsStopped)
    {
        if (!MessageToBeSend.IsEmpty())
        {
            FString Message;
            MessageToBeSend.Dequeue(Message);
            UdpServerUtilities::SendDataWithUdp(OurReuseAbleSocket, OurReuseAbleRemoteAddress.Get(), Message);
        }
        else
        {
            FPlatformProcess::Sleep(0.05f);
        }
    }
    return 0;
}

void FSendCurrentDegreeRunnable::Enqueue(const FString& Message)
{
    MessageToBeSend.Enqueue(Message);
}

void FSendCurrentDegreeRunnable::Stop()
{
    CurrentThreadIsStopped = true;
}

void FSendCurrentDegreeRunnable::Exit()
{
    OurReuseAbleSocket->Close();
    ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(OurReuseAbleSocket);
}
