// Fill out your copyright notice in the Description page of Project Settings.


#include "UdpServerUtilities.h"

#include "UdpServerInfo.h"
TMap<FString, EOperationType> UdpServerUtilities::OperationMap = TMap<FString, EOperationType>();

//读取Json字符串
bool UdpServerUtilities::GetJsonObjectFromJsonFString(const FString& _jsonFString, TSharedPtr<FJsonObject>& _jsonObject)
{
    if (!_jsonFString.IsEmpty())
    {
        TSharedRef<TJsonReader<>> t_reader = TJsonReaderFactory<>::Create(_jsonFString);
        return FJsonSerializer::Deserialize(t_reader, _jsonObject);
    }
    return false;
}

EOperationType UdpServerUtilities::ToEnumType(const FString& Str)
{
    return OperationMap[Str.ToUpper()];
}

FVector2D UdpServerUtilities::ToPCLocation(FVector2D PhoneLocation)
{
    float X = FMath::SmoothStep(0, PhonePCScreenSize::PhoneScreenSize.X, PhoneLocation.X) *
        PhonePCScreenSize::PCScreenSize.X;
    float Y = FMath::SmoothStep(0, PhonePCScreenSize::PhoneScreenSize.Y, PhoneLocation.Y) *
        PhonePCScreenSize::PCScreenSize.Y;
    return FVector2D(X, Y);
}


void UdpServerUtilities::InitOperationMap()
{
    OperationMap.Add(TEXT("FLY_MODE_START"), EOperationType::FLY_MODE_START);
    OperationMap.Add(TEXT("FLY_MODE_DRAG"), EOperationType::FLY_MODE_DRAG);
    OperationMap.Add(TEXT("FLY_MODE_SCALE"), EOperationType::FLY_MODE_SCALE);
    OperationMap.Add(TEXT("FLY_MODE_ROTATE"), EOperationType::FLY_MODE_ROTATE);

    OperationMap.Add(TEXT("GROUND_MODE_MOVE"), EOperationType::GROUND_MODE_MOVE);

    OperationMap.Add(TEXT("GROUND_MODE_JUMP"), EOperationType::GROUND_MODE_JUMP);
    OperationMap.Add(TEXT("GROUND_MODE_START_SPRINT"), EOperationType::GROUND_MODE_START_SPRINT);
    OperationMap.Add(TEXT("GROUND_MODE_STOP_SPRINT"), EOperationType::GROUND_MODE_STOP_SPRINT);

    OperationMap.Add(TEXT("GROUND_MODE_CAMERA_MOVE"), EOperationType::GROUND_MODE_CAMERA_MOVE);
}

bool UdpServerUtilities::SendDataWithUdp(const FString& Message, const FString& TheIP, const uint32 ThePort,
                                         const FString& YourChosenSocketName)
{
    TSharedPtr<FInternetAddr> RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    bool bIsValid;
    RemoteAddr->SetIp(*TheIP, bIsValid);
    RemoteAddr->SetPort(ThePort);
    if (!bIsValid)
    {
        //ScreenMsg("Rama UDP Sender>> IP address was not valid!", TheIP);  
        return false;
    }
    FSocket* SenderSocket = FUdpSocketBuilder(*YourChosenSocketName)
                            .AsReusable()
                            .WithBroadcast() /////////////广播  
                            .WithSendBufferSize(1024);
    //check(SenderSocket->GetSocketType() == SOCKTYPE_Datagram);  
    //Set Send Buffer Size  
    int32 SendSize = 1024;
    SenderSocket->SetSendBufferSize(SendSize, SendSize);
    SenderSocket->SetReceiveBufferSize(SendSize, SendSize);
    //return bIsValid;    
    //~~~~~~~~~~~~~~~~  
    //发送消息  
    int32 BytesSent = 0;
    FString serialized = Message;
    TCHAR* serializedChar = serialized.GetCharArray().GetData();
    int32 size = FCString::Strlen(serializedChar);
    int32 sent = 0;
    //SenderSocket->SendTo(Writer.GetData(), Writer.Num(), BytesSent, *RemoteAddr);  
    SenderSocket->SendTo((uint8*)TCHAR_TO_UTF8(serializedChar), size, BytesSent, *RemoteAddr); //发送给远端地址  
    if (BytesSent <= 0)
    {
        const FString Str = "Socket is valid but the receiver received 0 bytes, make sure it is listening properly!";
        UE_LOG(LogTemp, Error, TEXT("%s"), *Str);
        //ScreenMsg(Str);  
        return false;
    }
    //ScreenMsg("UDP Send Succcess! INFO Sent = ", ToSend);
    SenderSocket->Close();
    ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(SenderSocket);
    GEngine->AddOnScreenDebugMessage(-1,100.f,FColor::Red,"Send"+Message);
    return true;
}

bool UdpServerUtilities::GetFStringInJsonFormat(const TSharedPtr<FJsonObject>& _ptrJsonObj, FString& _strGet)
{
    if (_ptrJsonObj.IsValid() && _ptrJsonObj->Values.Num() > 0)
    {
        TSharedRef<TJsonWriter<TCHAR>> t_writer = TJsonWriterFactory<>::Create(&_strGet);
        FJsonSerializer::Serialize(_ptrJsonObj.ToSharedRef(), t_writer);
        return true;
    }
    return false;
}
