// Fill out your copyright notice in the Description page of Project Settings.


#include "ScaleUdpServer.h"

#include "InputPawn.h"

void AScaleUdpServer::BeginPlay()
{
    Super::BeginPlay();
    bool isSuccess;
    StartUDPReceiver("DragUDPServer", "192.168.1.1", EOperationPort::FLY_MODE_SCALE, isSuccess);
    if (!isSuccess)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "Init falied!");
    }
}

void AScaleUdpServer::Handle(const TSharedPtr<FJsonObject>& JsonObject)
{
    FString Type;
    if (JsonObject->TryGetStringField("type", Type))
    {
        switch (ToEnumType(Type))
        {
        case EOperationType::FLY_MODE_START:
            {
                float X = JsonObject->GetNumberField("coordinateX");
                float Y = JsonObject->GetNumberField("coordinateY");
                FVector2D PCLocation = ToPCLocation(FVector2D(X, Y));
                //按下三个鼠标按键
                for (auto& IIputBase : AInputPawn::inputListeners)
                {
                    IIputBase->OnMouseLButtonDown(PCLocation);
                    IIputBase->OnMouseMidButtonDown(PCLocation);
                    IIputBase->OnMouseRButtonDown(PCLocation);
                }
                LastRotatePosition = PCLocation;
                break;
            }
        case EOperationType::FLY_MODE_SCALE:
            {
                float X = JsonObject->GetNumberField("coordinateX");
                float Y = JsonObject->GetNumberField("coordinateY");
                float ScaleRate = JsonObject->GetNumberField("degree");
                float Value = FMath::Sign(ScaleRate - 1.0f);
                for (auto& IInputBase : AInputPawn::inputListeners)
                {
                    IInputBase->OnMouseWheel(ToPCLocation(FVector2D(X, Y)), Value);
                }
                break;
            }
        default:
            break;
        }
    }
}
