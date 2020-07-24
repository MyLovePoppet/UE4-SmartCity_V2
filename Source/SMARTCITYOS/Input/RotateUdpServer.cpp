// Fill out your copyright notice in the Description page of Project Settings.


#include "RotateUdpServer.h"

#include "InputPawn.h"

bool ARotateUdpServer::InitOurSocket(const FString& TheIP, const uint32 ThePort, const FString& YourChosenSocketName)
{
    OurReuseAbleRemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    bool bIsValid;
    OurReuseAbleRemoteAddress->SetIp(*TheIP, bIsValid);
    OurReuseAbleRemoteAddress->SetPort(ThePort);
    if (!bIsValid)
    {
        //ScreenMsg("Rama UDP Sender>> IP address was not valid!", TheIP);
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
                                         TEXT("Rama UDP Sender>> IP address was not valid!"));
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
    return bIsValid;
}

void ARotateUdpServer::BeginPlay()
{
    Super::BeginPlay();
    InitOurSocket();
    bool isSuccess;
    StartUDPReceiver("RotateUDPServer", "127.0.0.1", EOperationPort::FLY_MODE_ROTATE, isSuccess);
    if (!isSuccess)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "Rotate server Init falied!");
    }
}

void ARotateUdpServer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    OurReuseAbleSocket->Close();
    ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(OurReuseAbleSocket);
}

void ARotateUdpServer::Handle(const TSharedPtr<FJsonObject>& JsonObject)
{
    FString Type;
    if (JsonObject->TryGetStringField("type", Type))
    {
        switch (UdpServerUtilities::ToEnumType(Type))
        {
        case EOperationType::FLY_MODE_ROTATE:
            {
                float X = JsonObject->GetNumberField("coordinateX");
                float Y = JsonObject->GetNumberField("coordinateY");
                float RotateAngle = JsonObject->GetNumberField("degree");
                for (auto& IInputBase : AInputPawn::inputListeners)
                {
                    //转化为移动的数据
                    RotateAngle = FMath::Clamp(RotateAngle, -5.f, 5.f);
                    //转化为弧度制进行旋转
                    const float RotateRadian = FMath::DegreesToRadians(RotateAngle);
                    IInputBase->Rotate(UdpServerUtilities::ToPCLocation(FVector2D(X, Y)), RotateRadian);
                    //当前的旋转角度
                    CurrentDegree -= RotateAngle;
                    //固定到0~360度内
                    if (CurrentDegree >= 360.f)
                        CurrentDegree -= 360.f;
                    if (CurrentDegree < 0)
                        CurrentDegree += 360.f;
                    FString SendBackStr = SendBackMessage::MESSAGE_SEND_BACK_CURRENT_DEGREE.Replace(
                        *FString("?"), *FString::SanitizeFloat(CurrentDegree));
                    //GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, SendBackStr);
                    //回发数据
                    //UdpServerUtilities::SendDataWithUdp(SendBackStr,TEXT("192.168.50.247"));
                    UdpServerUtilities::SendDataWithUdp(OurReuseAbleSocket, OurReuseAbleRemoteAddress.Get(),
                                                        SendBackStr);
                }
                break;
            }
        case EOperationType::FLY_MODE_RETURN_NORTH:
            {
                CurrentDegree = 0.f;
                FString SendBackStr = SendBackMessage::MESSAGE_SEND_BACK_CURRENT_DEGREE.Replace(
                    *FString("?"), *FString::SanitizeFloat(CurrentDegree));
                //GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, SendBackStr);
                //回发数据
                //UdpServerUtilities::SendDataWithUdp(SendBackStr,TEXT("192.168.50.247"));

                //@Todo 回到正北方向

                UdpServerUtilities::SendDataWithUdp(OurReuseAbleSocket, OurReuseAbleRemoteAddress.Get(),
                                                    SendBackStr);
                GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "Return to north");
            }
        case EOperationType::FLY_MODE_START:
            {
                // float X = JsonObject->GetNumberField("coordinateX");
                // float Y = JsonObject->GetNumberField("coordinateY");
                // FVector2D PCLocation = UdpServerUtilities::ToPCLocation(FVector2D(X, Y));
                // //FVector2D PCLocation = PhonePCScreenSize::PhoneScreenSize / 2.f;
                // //按下三个鼠标按键
                // for (auto& IIputBase : AInputPawn::inputListeners)
                // {
                //     IIputBase->OnMouseLButtonDown(PCLocation);
                //     //IIputBase->OnMouseLButtonUp(PCLocation);
                //     IIputBase->OnMouseMidButtonDown(PCLocation);
                //     IIputBase->OnMouseRButtonDown(PCLocation);
                // }
                // LastRotatePosition = PCLocation;
                // break;
            }

        default:
            break;
        }
    }
}
