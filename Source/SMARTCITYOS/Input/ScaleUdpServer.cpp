// Fill out your copyright notice in the Description page of Project Settings.


#include "ScaleUdpServer.h"

#include "InputPawn.h"

void AScaleUdpServer::BeginPlay()
{
    Super::BeginPlay();
    bool isSuccess;
    StartUDPReceiver("ScaleUDPServer", "127.0.0.1", EOperationPort::FLY_MODE_SCALE, isSuccess);
    if (!isSuccess)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "Scale server Init falied!");
    }
}

void AScaleUdpServer::Handle(const TSharedPtr<FJsonObject>& JsonObject)
{
    FString Type;
    if (JsonObject->TryGetStringField("type", Type))
    {
        switch (UdpServerUtilities::ToEnumType(Type))
        {
        case EOperationType::FLY_MODE_SCALE:
            {
                float X = JsonObject->GetNumberField("coordinateX");
                float Y = JsonObject->GetNumberField("coordinateY");
                float ScaleRate = JsonObject->GetNumberField("degree");
                float Value = FMath::Sign(ScaleRate - 1.0f);
                for (auto& IInputBase : AInputPawn::inputListeners)
                {
                    IInputBase->OnMouseWheel(UdpServerUtilities::ToPCLocation(FVector2D(X, Y)), Value / 3.f);
                    //IInputBase->Scale( Value/5.f);
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
