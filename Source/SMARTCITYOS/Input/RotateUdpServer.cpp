// Fill out your copyright notice in the Description page of Project Settings.


#include "RotateUdpServer.h"

#include "InputPawn.h"

void ARotateUdpServer::BeginPlay()
{
    Super::BeginPlay();
    bool isSuccess;
    StartUDPReceiver("RotateUDPServer", "127.0.0.1", EOperationPort::FLY_MODE_ROTATE, isSuccess);
    if (!isSuccess)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "Rotate server Init falied!");
    }
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
                    // float XOffset = RotateAngle / 180.0f * PhonePCScreenSize::PCScreenSize.X;
                    // FVector2D CurrentLocation = LastRotatePosition + FVector2D(XOffset, 0.f);
                    // IInputBase->OnMouseMMove(CurrentLocation, 1.0f);
                    //LastRotatePosition = CurrentLocation;
                    IInputBase->Rotate(UdpServerUtilities::ToPCLocation(FVector2D(X, Y)), RotateAngle / 50.f);
                }
                break;
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
